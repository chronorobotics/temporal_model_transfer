import scipy.stats as st
import numpy as np
from dpgmm import dpgmm as dp
import sys


covariance_prior = np.array([[1, 0.5, 0.5],
                             [0.5, 1, 0.5],
                             [0.5, 0.5, 1]])
noon = 43200
midnight = 0


def generate_covariance(periodicities):
    size = 1 + 2*len(periodicities)
    ret = np.full((size, size), 0)
    for i in range(size):
        ret[i][i] = 15
    return ret


means_day = [np.array([0, np.cos(noon * 2 * np.pi / 86400.0), np.sin(noon * 2 * np.pi / 86400.0)]),
             np.array([1, np.cos(0), np.sin(0)])]


class Doors_DPGMM:
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
    one_in = 50

    def __init__(self, periodicities, data_dimension):
        # self.clusters = clusters
        # self.structure = structure
        self.dimensions = data_dimension
        self.periodicities = periodicities

        self.eps = .01
        self.corrective = 1.0

        means_priors = [
            self._create_X(np.array([[noon, 1]]))[0],
            self._create_X(np.array([[midnight, 0]]))[0],
        ]

        covariations_priors = [
            generate_covariance(self.periodicities),
            generate_covariance(self.periodicities)
        ]
        space_dim = self.dimensions + 2*len(self.periodicities)
        self.dpgmm_1 = dp.DPGMM(space_dim)
        self.dpgmm_1.setPrior(mean=means_priors[0], covar=covariations_priors[0], safe=False)
        #self.dpgmm_1.add(means_priors[0])
        #self.dpgmm_1.solve()

        self.dpgmm_0 = dp.DPGMM(space_dim)
        self.dpgmm_0.setPrior(mean=means_priors[1], covar=covariations_priors[1], safe=False)
        #self.dpgmm_0.add(means_priors[1])
        #self.dpgmm_0.solve()

        self.solve_counter = 0

    def fit(self, training_path):
        """
        objective:
            to train model
        input:
            training_path ... string, path to training dataset
        output:
            self
        """
        X = self._projection(training_path, 1)
        i = 0
        one_in = Doors_DPGMM.one_in
        for data in X:
            if i % one_in == 0:
                self.dpgmm_1.add(data)
            i += 1
        self.dpgmm_1.solveGrow()

        X = self._projection(training_path, 0)
        i = 0
        for data in X:
            if i % one_in == 0:
                self.dpgmm_0.add(data)
            i += 1
        self.dpgmm_0.solveGrow()

        return self

    def fit_data(self, dataset):
        until = 200
        self.solve_counter = 1

        # print(dataset)
        ones = dataset[dataset[:, -1] == 1]
        X = self._create_X(ones)
        i = 0
        one_in = Doors_DPGMM.one_in
        for data in X:
            # if i % one_in == 0:
            self.dpgmm_1.add(data)
            # i += 1
        if len(ones) != 0:
            # if self.dpgmm_1.size() < until:
            if  self.solve_counter == 2:
                # sys.stderr.write("h\n")
                self.dpgmm_1.solveGrow(5)
            else:
                self.dpgmm_1.solve()

        # if self.dpgmm_1.size() - self.dpgmm_1.skip > lock_size:
        #     self.dpgmm_1.lock(self.dpgmm_1.size())

        zeros = dataset[dataset[:, -1] == 0]
        X = self._create_X(zeros)
        i = 0
        for data in X:
            # if i % one_in == 0:
            self.dpgmm_0.add(data)
            # i += 1
        if len(zeros) != 0:
            # if self.dpgmm_0.size() < until:
            if  self.solve_counter == 2:
                # sys.stderr.write("h\n")
                self.dpgmm_0.solveGrow(5)
            else:
                self.dpgmm_0.solve()

        # if self.dpgmm_0.size() - self.dpgmm_0.skip > lock_size:
        #     self.dpgmm_0.lock(self.dpgmm_0.size())

        # sys.stderr.write(str(self.size()) + "\n")
        return self

    def size(self):
        return self.dpgmm_0.size() + self.dpgmm_1.size()

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

    def set_eps(self, epsilon):
        self.eps = epsilon

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
        pass

    def _update(self, data):
        pass

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
        dataset = np.loadtxt(path)
        if condition is None:
            return self._create_X(dataset)
        else:
            X = self._create_X(dataset[dataset[:, -1] == condition])
        return X

    def predict(self, X):
        """
        objective:
            return predicted values
        input:
            X ... np.array, test dataset transformed into the hypertime space
        output:
            prediction ... probability of the occurrence of detections
        """
        # print(X)
        #X = X[0]
        # print X
        # print self.dpgmm_0.getDM()
        # print self.dpgmm_1.getDM()
        vec = np.empty((len(X), self.dimensions + len(X[0])))
        vec[:, self.dimensions:] = X
        vec[:, 0] = 1
        prob_1_1 = np.asarray([self.dpgmm_1.prob(vec_item) for vec_item in vec])
        # vec[0] = 0.75
        # prob_1_075 = self.dpgmm_1.prob(vec)
        # vec[0] = 0.5
        # prob_1_05 = self.dpgmm_1.prob(vec)
        # vec[0] = 0.25
        # prob_1_025 = self.dpgmm_1.prob(vec)
        #vec[0] = 0
        #prob_1_0 = self.dpgmm_1.prob(vec)

        # vec[0] = 1
        # prob_0_1 = self.dpgmm_0.prob(vec)
        # vec[0] = 0.75
        # prob_0_075 = self.dpgmm_0.prob(vec)
        # vec[0] = 0.5
        # prob_0_05 = self.dpgmm_0.prob(vec)
        # vec[0] = 0.25
        # prob_0_025 = self.dpgmm_0.prob(vec)
        vec[:, 0] = 0
        prob_0_0 = np.asarray([self.dpgmm_0.prob(vec_item) for vec_item in vec])


        positives = self.dpgmm_1.size()
        negatives = self.dpgmm_0.size()



        res = np.empty(len(vec))
        #greater_eps = np.logical_and(prob_1_1 > prob_0_0, abs(prob_1_1 - prob_0_0) > self.eps)
        #greater_eps = np.logical_and(prob_1_1 > prob_1_0, abs(prob_1_1 - prob_1_0) > self.eps)
        d = positives * prob_1_1 + negatives * prob_0_0
        res = np.where(d != 0, (self.corrective * positives * prob_1_1)/d,
                       np.where(prob_1_1 > prob_0_0, prob_1_1, prob_0_0))
        #res[greater_eps] = 1
        #res[np.logical_not(greater_eps)] = 0

        with open('pred.txt', 'w+') as f:
            for item in res:
                f.write("%s\n" % item)

        return res

        # DISTR_1 = []
        # DISTR_0 = []
        # #samples = self.number_samples
        # for idx in xrange(1):
        #     DISTR_1.append(self.dpgmm_1.prob(X))
        #     DISTR_0.append(self.dpgmm_0.prob(X))
        # DISTR_1 = np.array(DISTR_1)
        # DISTR_0 = np.array(DISTR_0)
        # model_1_s = np.sum(DISTR_1, axis=0)
        # model_0_s = np.sum(DISTR_0, axis=0)
        # model_01_s = model_1_s + model_0_s
        # model_01_s[model_01_s == 0] = 1.0
        # model_1_s[model_01_s == 0] = 0.5
        # y = model_1_s / model_01_s
        # return y

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
        pass

    def project_time(self, time):
        dim = 0
        wavelengths = self.periodicities
        if len(time) == self.dimensions:
            # print "a"
            X = np.empty((self.dimensions, (len(wavelengths) * 2)))
        elif isinstance(time[0], list) and len(time[0]) == self.dimensions:
            # print "b"
            X = np.empty((len(time), (len(wavelengths) * 2)))
        elif self.dimensions == 1:
            # print "c"
            X = np.empty((len(time), (len(wavelengths) * 2)))
            # time = [[t] for t in time]
        else:
            raise ValueError("project_time() received unrecognised format of data")

        for Lambda in wavelengths:
            X[:, dim: dim + 2] = np.c_[np.cos(time * 2 * np.pi / Lambda),
                                       np.sin(time * 2 * np.pi / Lambda)]
            dim = dim + 2
        return X

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

