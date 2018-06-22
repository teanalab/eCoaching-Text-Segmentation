import logging
import os
import numpy as np
from deepecoach.models.ss_model import SSModel
from deepecoach.utils import unvectorize

logger = logging.getLogger(__name__)


class TrainManager:
    def __init__(self, lexical_model, lexical_params, strategy, batch_size, task):
        self.lexical_model = lexical_model
        self.lexical_params = lexical_params
        self.strategy = strategy
        self.batch_size = batch_size
        self.task = task
        self.l_instance = None
        self.c_instance = None
        self.best_p = None

    def _get_model(self):
        if self.task == 'dd_fillers':
            return FillerModel
        elif self.task == 'dd_editdisfs' or self.task == 'dd_editdisfs_binary':
            return EditDisfModel
        return SSModel

    def train(self, ds_train, ds_test=None, ds_val=None, nb_epoch=20, verbose=True):
        lexical_stats = [0, 0, 0]
        combiner_stats = [0, 0, 0]
        best_partition = 1

        Model = self._get_model()
        if self.lexical_model is not None:
            self.l_instance = Model(self.lexical_model, self.strategy, batch_size=self.batch_size)
            self.l_instance.build(**self.lexical_params)
            self.l_instance.train(ds_train, ds_test, ds_val=ds_val, nb_epoch=nb_epoch, verbose=verbose)
            # lexical_stats = self.l_instance.evaluate(ds_test)
            lexical_stats = self.l_instance.test_stats.stats
            combiner_stats = lexical_stats
            best_partition = 1
            if verbose:
                self.l_instance.test_stats._show_statistics(*lexical_stats)

        return lexical_stats, prosodic_stats, combiner_stats, best_partition

    def evaluate(self, ds_test, verbose=True, vary_p=False):
        c_pred = None
        if self.l_instance is not None:
            l_data, _, l_gold = self.l_instance.model.prepare_input(ds_test, self.strategy)
            l_f1, l_p, l_r, l_pred, gold = self.l_instance.evaluate(l_data, l_gold, return_predictions=True,
                                                                    return_golds=True, verbose=verbose)
            c_pred = l_pred
            if verbose:
                logger.info('Lexical stats: ')
                self._show_statistics(l_f1, l_p, l_r)
        if self.c_instance is not None:
            print('')
            trained_p = self.c_instance.best_p
            if vary_p:
                step = 0.05
                for p in np.arange(0, 1 + step, step).tolist():
                    self.c_instance.best_p = p
                    self.c_instance.evaluate(l_pred, p_pred, gold, verbose=verbose)
            self.c_instance.best_p = trained_p
            logger.info('Best p: %.2f' % self.c_instance.best_p)
            self.c_instance.evaluate(l_pred, p_pred, gold, verbose=verbose)
            c_pred = self.c_instance.predict(l_pred, p_pred)
        if isinstance(c_pred[0][0], (list, np.ndarray)):
            c_pred = list(map(unvectorize, c_pred))
        return c_pred

    def _show_statistics(self, f1, prec, rec):
        logger.info('Precision: %.4f' % prec)
        logger.info('Recall   : %.4f' % rec)
        logger.info('F-Measure: %.4f' % f1)

    def get_test_predictions(self):
        predictions = []
        if self.l_instance is not None:
            predictions = self.l_instance.test_stats.best_pred
        if self.c_instance is not None:
            best_lexical = self.l_instance.test_stats.best_pred
            best_prosodic = self.p_instance.test_stats.best_pred
            predictions = self.c_instance.predict(best_lexical, best_prosodic)
        if isinstance(predictions[0][0], (list, np.ndarray)):
            predictions = list(map(unvectorize, predictions))
        return predictions

    def save(self, dirname):
        if self.l_instance is not None:
            # self.l_instance.save_weights()
            os.rename('data/models/train_lexical_weights.h5', dirname + 'train_lexical_weights.h5')
            if os.path.exists('data/models/test_lexical_weights.h5'):
                os.rename('data/models/test_lexical_weights.h5', dirname + 'test_lexical_weights.h5')
            if os.path.exists('data/models/val_lexical_weights.h5'):
                os.rename('data/models/val_lexical_weights.h5', dirname + 'val_lexical_weights.h5')
        if self.p_instance is not None:
            # self.p_instance.save_weights()
            os.rename('data/models/train_prosodic_weights.h5', dirname + 'train_prosodic_weights.h5')
            if os.path.exists('data/models/test_prosodic_weights.h5'):
                os.rename('data/models/test_prosodic_weights.h5', dirname + 'test_prosodic_weights.h5')
            if os.path.exists('data/models/val_prosodic_weights.h5'):
                os.rename('data/models/val_prosodic_weights.h5', dirname + 'val_prosodic_weights.h5')
        if self.c_instance is not None:
            self.c_instance.save(dirname + 'combiner.json')

    def load(self, dirname, verbose=True):
        Model = self._get_model()
        if self.lexical_model is not None:
            self.l_instance = Model(self.lexical_model, self.strategy, batch_size=self.batch_size)
            self.l_instance.build(verbose=verbose, **self.lexical_params)
            self.l_instance.load_weights(dirname + 'train_lexical_weights.h5')
        # model.load_weights('my_model_weights.h5')
