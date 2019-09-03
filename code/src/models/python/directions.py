from sklearn.mixture import GaussianMixture
import scipy.stats as st
import numpy as np

#import transformation_with_dirs as tr


class Directions:
    """
    parameters:
        clusters ... int, number of clusters created by method
        structure ... list, parameters of hypertime space
    attributes:
        clusters ... int, number of clusters created by method
        structure ... list, parameters of hypertime space
        C_1 ... np.array, centres of clusters from the detections's model
        Pi_1 ... np.array, weights of clusters from the detections's model
        PREC_1 ... np.array, precision matrices of clusters from the detections's model
        C_0 ... np.array, centres of clusters from the not-detections's model
        Pi_0 ... np.array, weights of clusters from the not-detections's model
        PREC_0 ... np.array, precision matrices of clusters from the not-detections's model
    methods:
        fit(training_path)
            objective:
                to train model
            input:
                training_path ... string, path to training dataset
            output:
                self
        transform_data(path)
            objective:
                return transformed data into warped hypertime space and also return target values (0 or 1)
            input:
                path ... string, path to the test dataset
            outputs:
                X ... np.array, test dataset transformed into the hypertime space
                target ... target values
        predict(X)
            objective:
                return predicted values
            input:
                X ... np.array, test dataset transformed into the hypertime space
            output:
                prediction ... probability of the occurrence of detections
        rmse(path)
            objective:
                return rmse between prediction and target of a test dataset
            inputs:
                path ... string, path to the test dataset
            output:
                err ... float, root of mean of squared errors
    """


    def __init__(self, clusters=3, structure=[2, [1.0, 1.0], [86400.0, 604800.0]]):
        self.clusters = clusters
        self.structure = structure


    def fit(self, training_path = '../data/two_weeks_days_nights_weekends_with_dirs.txt'):
        """
        objective:
            to train model
        input:
            training_path ... string, path to training dataset
        output:
            self
        """
        self.C_1, self.Pi_1, self.PREC_1 = self._estimate_distribution(1, training_path)
        self.C_0, self.Pi_0, self.PREC_0 = self._estimate_distribution(0, training_path)
        return self


    def transform_data(self, path, for_fremen=False):
        """
        objective:
            return transformed data into warped hypertime space and also return target values (0 or 1)
        input:
            path ... string, path to the test dataset
        outputs:
            X ... np.array, test dataset transformed into the hypertime space
            target ... target values
        """
        dataset=np.loadtxt(path)
        X = self._create_X(dataset[:, : -1])
        target = dataset[:, -1]
        if for_fremen:
            return X, target, dataset[:, 0]
        else:
            return X, target


    def _estimate_distribution(self, condition, path):
        """
        objective:
            return parameters of the mixture of gaussian distributions of the data from one class projected into the warped hypertime space
        inputs:
            condition ... integer 0 or 1, labels of classes, 0 for non-occurrences and 1 for occurrences
            path ... string, path to the test dataset
        outputs:
            C ... np.array, centres of clusters, estimation of expected values of each distribution
            Pi ... np.array, weights of clusters
            PREC ... np.array, precision matrices of clusters, inverse matrix to the estimation of the covariance of the distribution
        """
        X = self._projection(path, condition)
        clf = GaussianMixture(n_components=self.clusters, max_iter=500).fit(X)
        C = clf.means_
        labels = clf.predict(X)
        PREC = self._recalculate_precisions(X, labels)
        Pi = clf.weights_
        return C, Pi, PREC

    def _projection(self, path, condition):
        """
        objective:
            return data projected into the warped hypertime
        inputs:
            condition ... integer 0 or 1, labels of classes, 0 for non-occurrences and 1 for occurrences
            path ... string, path to the test dataset
        outputs:
            X ... numpy array, data projection
        """
        dataset=np.loadtxt(path)
        X = self._create_X(dataset[dataset[:, -1] == condition, : -1])
        return X


    def _recalculate_precisions(self, X, labels):
        """
        objective:
            return precision matices of each cluster (inverse matrices to the covariance matices)
        imputs:
            X ... numpy array, data projection
            labels ... numpy array, predicted labels for the data samples using GaussianMixture
        output:
            PREC ... numpy array, precision matrices
        """
        COV = []
        for i in xrange(self.clusters):
            COV.append(np.cov(X[labels == i].T))
        COV = np.array(COV)
        PREC = np.linalg.inv(COV)
        return PREC


    def predict(self, X):
        """
        objective:
            return predicted values
        input:
            X ... np.array, test dataset transformed into the hypertime space
        output:
            prediction ... probability of the occurrence of detections
        """
        DISTR_1 = []
        DISTR_0 = []
        for idx in xrange(self.clusters):
            DISTR_1.append(self.Pi_1[idx] * self._prob_of_belong(X, self.C_1[idx], self.PREC_1[idx]))
            DISTR_0.append(self.Pi_0[idx] * self._prob_of_belong(X, self.C_0[idx], self.PREC_0[idx]))
        DISTR_1 = np.array(DISTR_1)
        DISTR_0 = np.array(DISTR_0)
        model_1_s = np.sum(DISTR_1, axis=0)
        model_0_s = np.sum(DISTR_0, axis=0)
        model_01_s = model_1_s + model_0_s
        model_01_s[model_01_s == 0] = 1.0
        model_1_s[model_01_s == 0] = 0.5
        y = model_1_s / model_01_s
        return y


    def _prob_of_belong(self, X, C, PREC):
        """
        massively inspired by:
        https://stats.stackexchange.com/questions/331283/how-to-calculate-the-probability-of-a-data-point-belonging-to-a-multivariate-nor

        objective:
            return 1 - "p-value" of the hypothesis, that values were "generated" by the (normal) distribution
            https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.chi2.html
        imputs:
            X ... np.array, test dataset transformed into the hypertime space
            C ... np.array, centres of clusters, estimation of expected values of each distribution
            PREC ... numpy array, precision matrices of corresponding clusters
        output
            numpy array, estimation of probabilities for each tested vector
        """
        X_C = X - C
        c_dist_x = []
        for x_c in X_C:
            c_dist_x.append(np.dot(np.dot(x_c.T, PREC), x_c))
        c_dist_x = np.array(c_dist_x)
        return 1 - st.chi2.cdf(c_dist_x, len(C))


    def rmse(self, path):
        """
        objective:
            return rmse between prediction and target of a test dataset
        inputs:
            path ... string, path to the test dataset
        output:
            err ... float, root of mean of squared errors
        """
        X, target = self.transform_data(path)
        y = self.predict(X)
        return np.sqrt(np.mean((y - target) ** 2.0))


    def _create_X(self, data):
        """
        objective:
            return data of one class projected into the warped hypertime space
        input: 
            data numpy array nxd*, matrix of measures IRL, where d* is number of measured variables
                                   the first column need to be timestamp
                                   last two columns can be phi and v, the angle and speed of human
        output: 
            X numpy array nxd, data projected into the warped hypertime space
        """
        dim = self.structure[0]
        wavelengths = self.structure[1]
        X = np.empty((len(data), dim + (len(wavelengths) * 2) + self.structure[2]*2))
        X[:, : dim] = data[:, 1: dim + 1]
        for Lambda in wavelengths:
            X[:, dim: dim + 2] = np.c_[np.cos(data[:, 0] * 2 * np.pi / Lambda),
                                       np.sin(data[:, 0] * 2 * np.pi / Lambda)]
            dim = dim + 2

        if self.structure[2]:
            X[:, dim: dim + 2] = np.c_[data[:, -1] * np.cos(data[:, -2]),
                                       data[:, -1] * np.sin(data[:, -2])]
        return X

