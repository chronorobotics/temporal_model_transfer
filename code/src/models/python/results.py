import pandas as pd
import numpy as np

#def RMSE(y, target):
#    return np.sqrt(np.mean((TKd - target_d) ** 2.0))
#
#def CHI2(y, dataset


######################
# RANSALU
#####################
TKd = np.loadtxt('/home/tom/pro/my/WHyTeS/data/ransalu/out_new_wednesday_thursday_days_with_angles_plus_reversed.txt')
TKn = np.loadtxt('/home/tom/pro/my/WHyTeS/data/ransalu/out_new_wednesday_thursday_nights_with_angles_plus_reversed.txt')
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[2:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[2:, -1]
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[2:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
TKnn = TKn[night]
target_nn = target_n[night]
print('RANSALU RMSE for days: ' + str(np.sqrt(np.mean((TKd - target_d) ** 2.0))))
print('RANSALU RMSE for nights: ' + str(np.sqrt(np.mean((TKnn - target_nn) ** 2.0))))
print('RANSALU RMSE for days+nights: ' + str(np.sqrt(np.mean((TKn - target_n) ** 2.0))))
print('')



######################
# CHITTARANJAN
#####################
TKd = np.loadtxt('/home/tom/pro/my/WHyTeS/data/chittaranjan/wednesday_thursday_days_with_angles_plus_reversed_cliffmap.txt')
TKn = np.loadtxt('/home/tom/pro/my/WHyTeS/data/chittaranjan/wednesday_thursday_nights_with_angles_plus_reversed_cliffmap.txt')
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, -1]
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
TKd[np.isnan(TKd)] = 0.0
TKn[np.isnan(TKn)] = 0.0
TKnn = TKn[night]
target_nn = target_n[night]
print('CHITTARANJAN RMSE for days: ' + str(np.sqrt(np.mean((TKd - target_d) ** 2.0))))
print('CHITTARANJAN RMSE for days+nights: ' + str(np.sqrt(np.mean((TKn - target_n) ** 2.0))))
print('CHITTARANJAN RMSE for nights: ' + str(np.sqrt(np.mean((TKnn - target_nn) ** 2.0))))
print('')




######################
# SERGI
#####################
TKd0 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_0p_step10min_predict_days_probabilities.txt')
TKn0 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_0p_step10min_predict_nights_probabilities.txt')
#TKd1 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_1p_step10min_predict_days_probabilities.txt')
#TKn1 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_1p_step10min_predict_nights_probabilities.txt')
TKd1 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_0p_step10min_predict_days_probabilities_no_norm.txt')
TKn1 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_0p_step10min_predict_nights_probabilities_no_norm.txt')
TKd2 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_2p_step10min_predict_days_probabilities.txt')
TKn2 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_2p_step10min_predict_nights_probabilities.txt')
TKd3 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_3p_step10min_predict_days_probabilities.txt')
TKn3 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_3p_step10min_predict_nights_probabilities.txt')
TKd4 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_4p_step10min_predict_days_probabilities.txt')
TKn4 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_4p_step10min_predict_nights_probabilities.txt')
#TKd5 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_5p_step10min_predict_days_probabilities.txt')
#TKn5 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_5p_step10min_predict_nights_probabilities.txt')
TKd5 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_4p_step10min_predict_days_probabilities_no_norm.txt')
TKn5 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/sergi_s_output/1m_10min_train_4p_step10min_predict_nights_probabilities_no_norm.txt')
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, -1]
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
TKnn0 = TKn0[night]
TKnn1 = TKn1[night]
TKnn2 = TKn2[night]
TKnn3 = TKn3[night]
TKnn4 = TKn4[night]
TKnn5 = TKn5[night]
target_nn = target_n[night]
print('SERGI RMSE for days, 0 periods (static model): ' + str(np.sqrt(np.mean((TKd0 - target_d) ** 2.0))))
print('SERGI RMSE for days+nights, 0 periods (static model): ' + str(np.sqrt(np.mean((TKn0 - target_n) ** 2.0))))
print('SERGI RMSE for nights, 0 periods (static model): ' + str(np.sqrt(np.mean((TKnn0 - target_nn) ** 2.0))))
print('')
print('SERGI RMSE for days, 0 period, no norm (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd1 - target_d) ** 2.0))))
print('SERGI RMSE for days+nights, 0 period, no norm (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn1 - target_n) ** 2.0))))
print('SERGI RMSE for nights, 0 period, no norm (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn1 - target_nn) ** 2.0))))
print('')
#print('SERGI RMSE for days, 1 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd1 - target_d) ** 2.0))))
#print('SERGI RMSE for days+nights, 1 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn1 - target_n) ** 2.0))))
#print('SERGI RMSE for nights, 1 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn1 - target_nn) ** 2.0))))
#print('')
#print('SERGI RMSE for days, 2 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd2 - target_d) ** 2.0))))
#print('SERGI RMSE for days+nights, 2 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn2 - target_n) ** 2.0))))
#print('SERGI RMSE for nights, 2 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn2 - target_nn) ** 2.0))))
#print('')
#print('SERGI RMSE for days, 3 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd3 - target_d) ** 2.0))))
#print('SERGI RMSE for days+nights, 3 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn3 - target_n) ** 2.0))))
#print('SERGI RMSE for nights, 3 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn3 - target_nn) ** 2.0))))
#print('')
print('SERGI RMSE for days, 4 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd4 - target_d) ** 2.0))))
print('SERGI RMSE for days+nights, 4 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn4 - target_n) ** 2.0))))
print('SERGI RMSE for nights, 4 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn4 - target_nn) ** 2.0))))
print('')
#print('SERGI RMSE for days, 5 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd5 - target_d) ** 2.0))))
#print('SERGI RMSE for days+nights, 5 periods (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn5 - target_n) ** 2.0))))
#print('SERGI RMSE for nights, 5 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn5 - target_nn) ** 2.0))))
#print('')
print('SERGI RMSE for days, 4 periods, no norm (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd5 - target_d) ** 2.0))))
print('SERGI RMSE for days+nights, 4 periods no norm (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn5 - target_n) ** 2.0))))
print('SERGI RMSE for nights, 4 period mo norm (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn5 - target_nn) ** 2.0))))
print('')


######################
# GORGE
#####################
TKd = np.loadtxt('/home/tom/pro/my/WHyTeS/data/george/wednesday_thursday_days_with_angles_plus_reversed_and_shuffled-50-50-fast.txt')[:, -1]
TKn = np.loadtxt('/home/tom/pro/my/WHyTeS/data/george/wednesday_thursday_nights_with_angles_plus_reversed_and_shuffled-50-50-fast.txt')[:, -1]
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[51:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[51:, -1]
#TKn = np.ones_like(target_n) * 0.22  # :)))
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[51:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
TKnn = TKn[night]
target_nn = target_n[night]
print('GORGE RMSE for days: ' + str(np.sqrt(np.mean((TKd - target_d) ** 2.0))))
print('GORGE RMSE for days+nights: ' + str(np.sqrt(np.mean((TKn - target_n) ** 2.0))))
print('GORGE RMSE for nights: ' + str(np.sqrt(np.mean((TKnn - target_nn) ** 2.0))))
print('')



#####################
# TOMAS
####################
TKd0 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/my_output/days_WHyTeS_3clusters_0periods.txt')
TKn0 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/my_output/nights_WHyTeS_3clusters_0periods.txt')
TKd2 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/my_output/days_WHyTeS_3clusters_3periods.txt')
TKn2 = np.loadtxt('/home/tom/pro/my/WHyTeS/data/my_output/nights_WHyTeS_3clusters_3periods.txt')
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, -1]
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
TKnn0 = TKn0[night]
TKnn2 = TKn2[night]
target_nn = target_n[night]
print('TOMAS RMSE for days, 0 periods (static model): ' + str(np.sqrt(np.mean((TKd0 - target_d) ** 2.0))))
print('TOMAS RMSE for days+nights, 0 periods (static model): ' + str(np.sqrt(np.mean((TKn0 - target_n) ** 2.0))))
print('TOMAS RMSE for nights, 0 periods (static model): ' + str(np.sqrt(np.mean((TKnn0 - target_nn) ** 2.0))))
print('')
print('TOMAS RMSE for days, 3 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKd2 - target_d) ** 2.0))))
print('TOMAS RMSE for days+nights, 3 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKn2 - target_n) ** 2.0))))
print('TOMAS RMSE for nights, 3 period (spatio-temporal model): ' + str(np.sqrt(np.mean((TKnn2 - target_nn) ** 2.0))))
print('')
print('p=1/8 for all, day, night, day and night: ' + str(np.sqrt(np.mean((np.ones_like(target_d)/8 - target_d) ** 2.0))) + str(np.sqrt(np.mean((np.ones_like(target_nn)/8 - target_nn) ** 2.0))) + str(np.sqrt(np.mean((np.ones_like(target_n)/8 - target_n) ** 2.0))))
print('')



#####################
# RANDOM, ZEROS, ETC
####################
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, -1]
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
target_nn = target_n[night]
print('MARIA RMSE for days, random 0 or 1: ' + str(np.sqrt(np.mean((np.random.rand(len(target_d)).round() - target_d) ** 2.0))))
print('MARIA RMSE for days+nights, random 0 or 1: ' + str(np.sqrt(np.mean((np.random.rand(len(target_n)).round() - target_n) ** 2.0))))
print('MARIA RMSE for nights, random 0 or 1: ' + str(np.sqrt(np.mean((np.random.rand(len(target_nn)).round() - target_nn) ** 2.0))))
print('')
print('MARIA RMSE for days, random between 0 and 1: ' + str(np.sqrt(np.mean((np.random.rand(len(target_d)) - target_d) ** 2.0))))
print('MARIA RMSE for days+nights, random between 0 and 1: ' + str(np.sqrt(np.mean((np.random.rand(len(target_n)) - target_n) ** 2.0))))
print('MARIA RMSE for nights, random between 0 and 1: ' + str(np.sqrt(np.mean((np.random.rand(len(target_nn)) - target_nn) ** 2.0))))
print('')
print('MARIA RMSE for days, zeros: ' + str(np.sqrt(np.mean((np.zeros_like(target_d) - target_d) ** 2.0))))
print('MARIA RMSE for days+nights, zeros: ' + str(np.sqrt(np.mean((np.zeros_like(target_n) - target_n) ** 2.0))))
print('MARIA RMSE for nights, zeros: ' + str(np.sqrt(np.mean((np.zeros_like(target_nn) - target_nn) ** 2.0))))
print('')
print('MARIA RMSE for days, ones: ' + str(np.sqrt(np.mean((np.ones_like(target_d) - target_d) ** 2.0))))
print('MARIA RMSE for days+nights, ones: ' + str(np.sqrt(np.mean((np.ones_like(target_n) - target_n) ** 2.0))))
print('MARIA RMSE for nights, ones: ' + str(np.sqrt(np.mean((np.ones_like(target_nn) - target_nn) ** 2.0))))
print('')


######################
# TOMEK
#####################
TKd = np.loadtxt('/home/tom/pro/my/WHyTeS/data/tomek_s_ouput/days_bin.csv')
TKn = np.loadtxt('/home/tom/pro/my/WHyTeS/data/tomek_s_ouput/nights_bin.csv')
target_d = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_days_with_angles_plus_reversed.txt')[:, -1]
target_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, -1]
time_n = np.loadtxt('/home/tom/pro/my/WHyTeS/data/wednesday_thursday_nights_with_angles_plus_reversed.txt')[:, 0]
night = (time_n % 86400.0 < 3600.0*7.0) | (time_n % 86400.0 > 3600.0*19.0)
TKnn = TKn[night]
target_nn = target_n[night]
print('TOMEK RMSE for days: ' + str(np.sqrt(np.mean((TKd - target_d) ** 2.0))))
print('TOMEK RMSE for days+nights: ' + str(np.sqrt(np.mean((TKn - target_n) ** 2.0))))
print('TOMEK RMSE for nights: ' + str(np.sqrt(np.mean((TKnn - target_nn) ** 2.0))))
print('')
