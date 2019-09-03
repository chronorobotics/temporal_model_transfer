from sklearn.mixture import GaussianMixture
import scipy.stats as st
import numpy as np

#import transformation_with_dirs as tr


covariance_prior = np.array([[1, 0.5, 0.5],
                             [0.5, 1, 0.5],
                             [0.5, 0.5, 1]])

noon = 43200

class Doors:
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

    # 43200

    def __init__(self):
        # self.clusters = clusters
        # self.structure = structure
        self.dimensions = 1
        self.periodicities = [86400.0]
        self.means_priors = [np.array([1, np.cos(noon * 2 * np.pi / 86400.0), np.sin(noon * 2 * np.pi / 86400.0)]),
                             np.array([0, np.cos(0), np.sin(0)])]
        self.means = self.means_priors
        self.covariations_priors = [
            np.linalg.inv(covariance_prior),     # stupid, but prior has to be inversed, even when not I
            np.linalg.inv(covariance_prior)
        ]
        self.covariations = [np.linalg.inv(self.covariations_priors[i]) for i in range(len(self.covariations_priors))]
        self.number_samples = [0, 0]
        print("init")

    def fit(self, training_path):
        """
        objective:
            to train model
        input:
            training_path ... string, path to training dataset
        output:
            self
        """
        self._estimate_distribution(1, training_path)
        self._estimate_distribution(0, training_path)
        return self

    def transform_data(self, path_data, path_times):
        """
        objective:
            return transformed data into warped hypertime space and also return target values (0 or 1)
        input:
            path ... string, path to the test dataset
        outputs:
            X ... np.array, test dataset transformed into the hypertime space
            target ... target values
        """
        data = np.loadtxt(path_data)
        times = np.loadtxt(path_times)
        dataset = np.dstack((times, data))[0]
        #target = data
        X = self._create_X(dataset)
        target = dataset[:, -1]
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
        i = 0
        for data in X:
            if True:  # i % 10 == 0:
                print (self.covariations)
                print(self.means)
                self._update(data)
            i += 1

        #clf = GaussianMixture(n_components=self.clusters, max_iter=500).fit(X)
        #C = clf.means_
        #labels = clf.predict(X)
        #PREC = self._recalculate_precisions(X, labels)
        #Pi = clf.weights_
        #return C, Pi, PREC

    def _update(self, data):
        value = int(data[0])
        idx = 1-value
        n = self.number_samples[idx]   # number of update samples
        covar = self.covariations[idx]
        covar_prior = self.covariations_priors[idx]
        mean_prior = self.means_priors[idx]
        inv = np.linalg.inv(covar)
        new_covar = np.linalg.inv(
            n*covar_prior + inv
        )
        a = np.dot(covar, mean_prior)
        b = np.dot(n * covar_prior, data)
        new_mean = np.dot(new_covar, (b + a))
        self.number_samples[idx] += 1
        self.means[1-value] = new_mean
        self.covariations[1-value] = np.linalg.inv(covar_prior) + new_covar

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
        X = self._create_X(dataset[dataset[:, -1] == condition])
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
        samples = self.number_samples
        for idx in xrange(2):
            DISTR_1.append(self._prob_of_belong(X, self.means[idx], self.covariations[idx]))
            DISTR_0.append(self._prob_of_belong(X, self.means[idx], self.covariations[idx]))
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
            return data projected into the warped hypertime space
        input:
            data numpy array nxd*, matrix of measures IRL, where d* is number of measured variables
                                   the first column need to be timestamp
                                   last two columns can be phi and v, the angle and speed of human
        output:
            X numpy array nxd, data projected into the warped hypertime space
        """
        dim = self.dimensions
        wavelengths = self.periodicities
        X = np.empty((len(data), dim + (len(wavelengths) * 2)))
        X[:, : dim] = data[:, 1: dim + 1]     # i don't have nor angle nor speed, my data is one dimensional
        for Lambda in wavelengths:
            X[:, dim: dim + 2] = np.c_[np.cos(data[:, 0] * 2 * np.pi / Lambda),
                                       np.sin(data[:, 0] * 2 * np.pi / Lambda)]
            dim = dim + 2
        return X

