import math

class Vector2:

	def __init__(self, x=0.0, y=0.0):
		if isinstance(x, tuple):
			self.x = x[0]
			self.y = x[1]
		else:
			self.x = float(x)
			self.y = float(y)

	def __add__(self, o):
		return Vector2(self.x + o.x, self.y + o.y)

	def __sub__(self, o):
		return Vector2(self.x - o.x, self.y - o.y)
	
	# glsl style simd multiplication
	def __mul__(self, f):
		if isinstance(f, int) or isinstance(f, float):
			return Vector2(self.x * f, self.y * f)
		elif(isinstance(f, Vector2)):
			return Vector2(self.x * f.x, self.y * f.y)
		elif(isinstance(f, Matrix2)):
			return f.__rmul__(self)
	
	__rmul__ = __mul__
	
	def __div__(self, f):
		if isinstance(f, float) or isinstance(f, int):
			return Vector2(self.x / f, self.y / f)
		elif isinstance(f, Vector2):
			return Vector2(self.x / f.x, self.y / f.y)
		else:
			assert(False)
	
	def __neg__(self):
		return Vector2(-self.x, -self.y)

	def norm2(self):
		return self.x * self.x + self.y * self.y

	def norm(self):
		return math.sqrt(self.norm2())

	def normalized(self):
		return self / self.norm() 
	
	def rotated90(self):
		return Vector2(-self.y, self.x)

	def __str__(self):
		return "(%f, %f)" % (self.x, self.y)
	
	def copy(self):
		return Vector2(self.x, self.y)
	
	def size(self):
		return 2
	
	def __getitem__(self, index):
		assert(index == 0 or index == 1)
		if index is 0:
			return self.x
		else:
			return self.y

	def __setitem__(self, index, value):
		assert(index == 0 or index == 1)
		if index is 0:
			self.x = value
		else:
			self.y = value
	
	def abs(self):
		return Vector2(abs(self.x), abs(self.y))


def dot(u, v):
	return u.x * v.x + u.y * v.y

def cross(u, v):
	return u.x * v.y - u.y * v.x

def normalize(vec):
	return vec.normalized()

def rotate90(vec):
	return vec.rotated90()

inner = dot

# return u * v.t
def outer(u, v):
	return Matrix2(u.x * v.x, u.y * v.x, u.x * v.y, u.y * v.y)


# 2x2 Matrix following the glsl mat2 
class Matrix2:

	# Either takes two col vectors to build the matrix, 
	# or takes 4 float 
	# [fc, c]
	# [sc, d]
	def __init__(self, fc=Vector2(), sc=Vector2(), c=None, d=None):
		if c is not None and d is not None:
			self.fc = Vector2(fc, sc)
			self.sc = Vector2(c, d)
		else:
			self.fc = fc.copy()
			self.sc = sc.copy()
		assert(self.fc is not None)
		assert(self.sc is not None)
	
	@classmethod
	def Identity(cls):
		return Matrix2(1, 0, 0, 1)

	def __add__(self, o):
		return Matrix2(self.fc + o.fc, self.sc + o.sc)

	def __sub__(self, o):
		return Matrix2(self.fc - o.fc, self.sc - o.sc)

	def __mul__(self, o):
		if isinstance(o, Matrix2):
			fc = Vector2(self.fc.x * o.fc.x + self.sc.x * o.fc.y, self.fc.y * o.fc.x + self.sc.y * o.fc.y)
			sc = Vector2(self.fc.x * o.sc.x + self.sc.x * o.sc.y, self.fc.y * o.sc.x + self.sc.y * o.sc.y)
			return Matrix2(fc, sc)
		elif isinstance(o, Vector2): 
			v = o
			return Vector2(self.fc.x * v.x + self.sc.x * v.y, self.fc.y * v.x + self.sc.y * v.y)
		elif isinstance(o, float) or isinstance(o, int):
			return Matrix2(self.fc * o, self.sc * o)
	
	__matmul__ = __mul__

	# If o is a number, then it is a trivial multiplication
	# o cannot be Matrix2: automatic link to the classic __mul__ instead
	# If o is a Vector2, the glsl vec2 * mat2 multiplication: return (o.t * self).t
	def __rmul__(self, o):
		assert(not isinstance(o, Matrix2))
		if isinstance(o, float) or isinstance(o, int):
			return self * o
		elif isinstance(o, Vector2):
			assert(self.fc is not None)
			assert(self.sc is not None)
			assert(o is not None)
			return Vector2(dot(o, self.fc), dot(o, self.sc))
		assert(False)

	def __div__(self, f):
		return Matrix2(self.fc / f, self.sc / f)
	
	def __neg__(self):
		return Matrix2(-self.fc, -self.sc)
	
	def t(self):
		return Matrix2(Vector2(self.fc.x, self.sc.x), Vector2(self.fc.y, self.sc.y))
	
	def det(self):
		return cross(self.fc, self.sc)
	
	def inv(self):
		d = self.det()
		if d == 0:
			print('Warning, could not invert the matrix: ')
			print(self)
			return None
		else:
			fc = Vector2(self.sc.y, -self.fc.y)
			sc = Vector2(-self.sc.x, self.fc.x)
			res = Matrix2(fc, sc) / d
			return res

	def __str__(self):
		return "[%f, %f]\n[%f, %f]" % (self.fc.x, self.sc.x, self.fc.y, self.sc.y)
	
	def copy(self):
		return Matrix2(self.fc.copy(), self.sc.copy())
	
	def size(self):
		return (2, 2)

	# According to glsl
	# (column, row)
	def __getitem__(self, index):
		if type(index) is tuple:
			assert(index[0] is 0 or index[0] is 1)
			if index[0] is 0:
				return self.fc[index[1]]
			else:
				return self.sc[index[1]]
		elif type(index) is int:
			assert(index is 0 or index is 1)
			if index is 0:
				return self.fc
			else:
				return self.sc
		else:
			assert(False)

	# According to glsl
	# (column, row)
	def __setitem__(self, index, value):
		if isinstance(index, tuple):
			assert(index[0] is 0 or index[0] is 1)
			if index[0] is 0:
				self.fc[index[1]] = value
			else:
				self.sc[index[1]] = value
		elif isinstance(index, int):
			assert(index is 0 or index is 1)
			assert(isinstance(value, Vector2))
			if index is 0:
				self.fc = value
			else:
				self.sc = value
		else:
			assert(False)

def RotateMatrix(angle):
    res = Matrix2()
    res[0, 0] = cos(angle)
    res[1, 1] = cos(angle)
    res[1, 0] = sin(angle)
    res[0, 1] = -sin(angle)
    return res

class Tensor2:

	def __init__(self, fm, sm):
		self.fm = fm
		self.sm = sm
	
	def __add__(self, o):
		return Tensor2(self.fm + o.fm, self.sm + o.fm)

	def __sub__(self, o):
		return Tensor2(self.fm - o.fm, self.sm - o.fm)
	


	def copy(self):
		return Tensor2(self.fm.copy(), self.sm.copy())

class BoundingBox:

	def __init__(self, minv=Vector2(float('inf'), float('inf')), maxv=Vector2(float('-inf'), float('-inf'))):
		self.min_vector = minv
		self.max_vector = maxv
	
	def max(self):
		return self.max_vector
	
	def min(self):
		return self.min_vector
	
	def d(self):
		return self.max() - self.min()
	
	def center(self):
		return (self.min() + self.max()) * 0.5
	
	def insideStrict(self, point):
		return point.x > self.min().x and point.x < self.max().x and point.y > self.min().y and point.y < self.max().y	
