import pickle
import numpy as np
from sklearn import preprocessing

def main(a,b,c,d):

     
 f = open('C:\\intern\\NN\\linearregression1.pkl', 'rb')    
 clf=pickle.load(f)
 f.close()    
 X_train=np.array([a,b,c,d]).reshape(-1,4)
 scaler = preprocessing.Normalizer().fit(X_train)
 X_train_standard = scaler.transform(X_train)
 Y_pred = clf.predict(X_train_standard)	  
 #print(X_train_standard)
 return Y_pred