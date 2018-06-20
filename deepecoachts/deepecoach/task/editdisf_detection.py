import os, re
from deepecoach.loader import build_dataset_from_data
from deepecoach.models.utils import get_new_texts, convert_predictions_to_tuples
from deepecoach.task import Task


class EditDisfDetector(Task):

	def __init__(self, **kwargs):
		super(EditDisfDetector, self).__init__(**kwargs)
		self.options['task'] = 'dd_editdisfs_binary'

