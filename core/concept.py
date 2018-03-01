import numpy as np


################## CREAR
# Nota no crear de 1 dimension (rank 1 array)

a = np.random.randn(5,1) # Column vector
a = np.random.randn(1,5) # Row vector


################## ASEGURAMIENTO

assert(a.shape == (5,1))

# Si falla
a = a.reshape((5,1)) # Time constant

################# FUNCIONES
np.zeros(SHAPE)
np.exp(TENSOR)
np.log(TENSOR)
np.abs(TENSOR)
np.maximun(TENSOR)

z = np.dot(TENSOR,TENSOR)







############################### ALGORITHM

J   = 0
dw1 = 0
dw2 = 0
db  = 0

for i in range(iterations):

	############################### Single iteration of GD
	
	# Foward
	Z = np.dot(w.T, X) + b
	A = activation(Z) # Sigmoid
	J += loss(A, Y)  # crossentropy (not neccesary for backward)

	# Backward
	dz = A - Y # chain rule of d_Sigmoid * d_crossentropy
	dw = (1/m) * np.dot(X, dz.T) # dot or element-wise ?
	db = (1/m) * np.sum(dz)

	# update
	w -= lr * dw
	b -= lr * db




############################### EXAMPLE

x=np.array([[1, 2, 3]])

w=np.array([[ 0,  1,  2,  3,  4],
			[ 5,  6,  7,  8,  9],
			[10, 11, 12, 13, 14]])





x.dot(w)












