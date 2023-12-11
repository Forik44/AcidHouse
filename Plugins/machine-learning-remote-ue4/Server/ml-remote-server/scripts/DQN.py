
import unreal_engine as ue
from mlpluginapi import MLPluginAPI
import tensorflow as tf
import numpy as np
import os
import tempfile

from tf_agents.environments import py_environment
from tf_agents.environments import tf_environment
from tf_agents.environments import tf_py_environment
from tf_agents.environments import utils
from tf_agents.specs import array_spec
from tf_agents.environments import wrappers
from tf_agents.environments import suite_gym
from tf_agents.trajectories import time_step as ts

from tf_agents.replay_buffers import tf_uniform_replay_buffer
from tf_agents.agents.categorical_dqn import categorical_dqn_agent
from tf_agents.agents.dqn import dqn_agent
from tf_agents.drivers import py_driver
from tf_agents.environments import suite_gym
from tf_agents.environments import tf_py_environment
from tf_agents.eval import metric_utils
from tf_agents.metrics import tf_metrics
from tf_agents.networks import sequential
from tf_agents.policies import py_tf_eager_policy
from tf_agents.policies import random_tf_policy
from tf_agents.networks import categorical_q_network
from tf_agents.replay_buffers import reverb_replay_buffer
from tf_agents.replay_buffers import reverb_utils
from tf_agents.trajectories import trajectory
from tf_agents.specs import tensor_spec
from tf_agents.utils import common
from tf_agents.policies import policy_saver
from tf_agents.drivers import dynamic_step_driver

tempdir = os.getenv("TEST_TMPDIR", tempfile.gettempdir())

collect_steps_per_iteration = 1# @param {type:"integer"}
replay_buffer_max_length = 100000  # @param {type:"integer"}

batch_size = 64  # @param {type:"integer"}
learning_rate = 1e-3  # @param {type:"number"}
log_interval = 10  # @param {type:"integer"}

num_eval_episodes = 300  # @param {type:"integer"}
eval_interval = 500  # @param {type:"integer"}

num_iterations = 10000 # @param {type:"integer"}

initial_collect_steps = 500  # @param {type:"integer"} 

replay_buffer_capacity = 100000  # @param {type:"integer"}

fc_layer_params = (100,50)

gamma = 0.99

num_atoms = 51  # @param {type:"integer"}
min_q_value = 0  # @param {type:"integer"}
max_q_value = 10000  # @param {type:"integer"}
n_step_update = 2  # @param {type:"integer"}

#MLPluginAPI
class DQN(MLPluginAPI):

	#optional api: setup your model for training
	def on_setup(self):
		ue.log('hello on_setup')

		self.ShEnv = ShooterEnv()
		self.env = tf_py_environment.TFPyEnvironment(self.ShEnv)

		ue.log('Observation Spec:')
		ue.log(self.env.time_step_spec().observation)

		ue.log('Reward Spec:')
		ue.log(self.env.time_step_spec().reward)

		ue.log('Action Spec:')
		ue.log(self.env.action_spec())
		
		num_actions = 5

		# Define a helper function to create Dense layers configured with the right
		# activation and kernel initializer.
		def dense_layer(num_units):
		  return tf.keras.layers.Dense(
			  num_units,
			  activation=tf.keras.activations.relu,
			  kernel_initializer=tf.keras.initializers.VarianceScaling(
				  scale=2.0, mode='fan_in', distribution='truncated_normal'))

		# QNetwork consists of a sequence of Dense layers followed by a dense layer
		# with `num_actions` units to generate one q_value per available action as
		# its output.
		dense_layers = [dense_layer(num_units) for num_units in fc_layer_params]
		q_values_layer = tf.keras.layers.Dense(num_actions,activation=None,kernel_initializer=tf.keras.initializers.RandomUniform(minval=-0.03, maxval=0.03),bias_initializer=tf.keras.initializers.Constant(-0.2))
		q_net = sequential.Sequential(dense_layers + [q_values_layer])

		optimizer = tf.keras.optimizers.Adam(learning_rate=learning_rate)

		categorical_q_net = categorical_q_network.CategoricalQNetwork(self.env.observation_spec(),self.env.action_spec(),num_atoms=num_atoms,fc_layer_params=fc_layer_params)

		train_step_counter = tf.Variable(0)

		self.agent = categorical_dqn_agent.CategoricalDqnAgent(
			self.env.time_step_spec(),
			self.env.action_spec(),
			categorical_q_network=categorical_q_net,
			optimizer=optimizer,
			min_q_value=min_q_value,
			max_q_value=max_q_value,
			n_step_update=n_step_update,
			td_errors_loss_fn=common.element_wise_squared_loss,
			gamma=gamma,
			train_step_counter=train_step_counter)


		#self.agent = dqn_agent.DqnAgent(env.time_step_spec(),env.action_spec(),q_network=q_net,optimizer=optimizer,td_errors_loss_fn=common.element_wise_squared_loss,train_step_counter=train_step_counter)
		#agent = self.agent
		#agent.initialize()

		self.random_policy = random_tf_policy.RandomTFPolicy(self.env.time_step_spec(),self.env.action_spec())
		pass
		
	#optional api: parse input object and return a result object, which will be converted to json for UE4
	def on_json_input(self, input):
		return {}#{'something':'something'}

	#optional api: start training your network
	def on_begin_training(self):
		ue.log('hello on_begin_training')
		agent = self.agent
		agent.initialize()
		random_policy = random_tf_policy.RandomTFPolicy(self.env.time_step_spec(),self.env.action_spec())

		replay_buffer = tf_uniform_replay_buffer.TFUniformReplayBuffer(
			data_spec=agent.collect_data_spec,
			batch_size=self.env.batch_size,
			max_length=replay_buffer_capacity)
		
		ue.log('replay_buffer created')

		def collect_step(environment, policy):
			time_step = self.env.current_time_step()
			#ue.log(self.env)
			action_step = policy.action(time_step)
			#ue.log('Env collect_step' + str(self.env))
			#ue.log(action_step)
			#self.call_event('action', str(action_step))
			next_time_step = self.env.step(action_step.action)
			traj = trajectory.from_transition(time_step, action_step, next_time_step)

			# Add trajectory to the replay buffer
			replay_buffer.add_batch(traj)
		
		def compute_avg_return(environment, policy, num_episodes):
			total_return = 0.0
			for _ in range(num_episodes):
				episode_return = 0.0
				time_step = self.env.current_time_step()
				#while not time_step.is_last():
				action_step = policy.action(time_step)
				ue.log(policy.action(time_step))
				next_time_step = self.env.step(action_step.action)
				episode_return += next_time_step.reward
				total_return += episode_return

			avg_return = total_return / num_episodes
			return avg_return.numpy()[0]

		for _ in range(initial_collect_steps):
			collect_step(self.env, random_policy)

		# This loop is so common in RL, that we provide standard implementations of
		# these. For more details see the drivers module.

		# Dataset generates trajectories with shape [BxTx...] where
		# T = n_step_update + 1.
		dataset = replay_buffer.as_dataset(
			num_parallel_calls=3, sample_batch_size=batch_size,
			num_steps=n_step_update + 1).prefetch(3)

		ue.log('dataset created')

		iterator = iter(dataset)

		ue.log('iterator created')
		# (Optional) Optimize by wrapping some of the code in a graph using TF function.
		agent.train = common.function(agent.train)

		# Reset the train step
		agent.train_step_counter.assign(0)
		checkpoint_dir = os.path.join(tempdir, 'checkpoint')
		policy_checkpointer = common.Checkpointer(ckpt_dir=checkpoint_dir,
                                          policy=agent.policy)
		
		ue.log('Checkpointer created')
		# Evaluate the agent's policy once before training.
		avg_return = compute_avg_return(self.env, agent.policy, num_eval_episodes)
		returns = [avg_return]
		policy_checkpointer.save(global_step=0)
		ue.log('Checkpointer saved global_step=0')

		file = open('D:/Unreal/Projects/AcidHouse/Plugins/machine-learning-remote-ue4/Server/ml-remote-server/scripts/Saved/Losses.txt', 'w')

		eval_policy = agent.policy
		collect_policy = agent.collect_policy

		for _ in range(num_iterations):
			# Collect a few steps using collect_policy and save to the replay buffer.
			for _ in range(collect_steps_per_iteration):
				collect_step(self.env, collect_policy)

			# Sample a batch of data from the buffer and update the agent's network.
			experience, unused_info = next(iterator)
			train_loss = agent.train(experience)

			step = agent.train_step_counter.numpy()

			if step % log_interval == 0:
				ue.log('step = {0}: loss = {1}'.format(step, train_loss.loss))

				file.write('step = {0}: loss = {1}\n'.format(step, train_loss.loss))

			if step % eval_interval == 0:
				avg_return = compute_avg_return(self.env, eval_policy, num_eval_episodes)
				ue.log('step = {0}: Average Return = {1:.2f}'.format(step, avg_return))
				#policy_checkpointer.save((global_step=step/eval_interval).numpy())
				returns.append(avg_return)

		file.write(str(returns))
		pass

	def set_reward(self, input):
		self.ShEnv._reward = input['reward']
		#ue.log('Env set reward' + str(self.env))
		return {}

	def set_state(self, input):
		self.ShEnv._state = input['state']
		#ue.log(str(self.env._state))
		return {}



#NOTE: this is a module function, not a class function. Change your CLASSNAME to reflect your class
#required function to get our api
def get_api():
	#return CLASSNAME.get_instance()
	return DQN.get_instance()

import time
class ShooterEnv(py_environment.PyEnvironment):

	def __init__(self):
		self._action_spec = array_spec.BoundedArraySpec(shape=(), dtype=np.int32, minimum=0, maximum=4, name='action')
		self._observation_spec = array_spec.BoundedArraySpec(shape=(1,), dtype=np.int32, minimum=0, name='observation')
		self._state = 0
		self._reward = 1
		self._current = 0

	def action_spec(self):
		return self._action_spec

	def observation_spec(self):
		return self._observation_spec

	def _reset(self):
		return ts.restart(np.array([self._state], dtype=np.int32))

	def SetReward(self, NewReward):
		self._reward = NewReward
	
	def DoAction(self, action):
		if action != self._current:
			self._current = action
			ue.log('action:' + str(action))
		pass

	def _step(self, action):
		self.DoAction(action)
		time.sleep(1)
		#ue.log('Reward:' + str(self._reward))
		#ue.log('State' + str(self._state))
		#ue.log('Env step' + str(self))
		return ts.termination(np.array([self._state], dtype=np.int32), self._reward)