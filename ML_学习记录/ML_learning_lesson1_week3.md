# 逻辑回归(Logistic Regression)

## 一、概念介绍

**1、逻辑回归与分类**

逻辑回归是一种经典的机器学习算法，虽然名称中包含“回归”，但实际上主要用于解决**分类问题**，尤其是**二分类问题**

**2、决策边界**

- 决策边界（也称为决策面）是一个**超曲面**，将特征空间（向量空间）划分为两个集合，每个集合对应于不同的分类。
- 在二分类问题中，决策边界是一个分隔不同类别的边界。

**3、代价函数**

- 代价函数是用于找到最优解的目标函数。它帮助我们衡量模型预测值与真实值之间的差异。
- 在机器学习中，我们通过调整模型参数来最小化代价函数，从而使模型更好地拟合训练数据。

**4、欠拟合与过拟合**

- 欠拟合对应于预测模型不能够实现对训练集的拟合，或者说拟合能力差

- 而过拟合对应于预测模型对训练集数据拟合的过于精准，这将导致模型在训练集中很完美，但在测试集中显露出不足，也即泛化能力差的特点

  <img src="G:\ML_learning\2022-Machine-Learning-Specialization-main\Supervised Machine Learning Regression and Classification\week3\QQ截图20240430222550.png" style="zoom:67%;" />

**5、正则化**

正则化是一种修改学习算法的方法，旨在减小泛化误差。通过在损失函数中添加一个正则项，对模型的参数施加限制，从而降低模型的复杂度。这样做的目的是防止模型过拟合，提高其在未知数据上的泛化能力。

****

## 二、算法和Python代码实现

### 逻辑梯度下降算法

代价函数为：$J(\vec{\mathrm{w}},b)=-\frac{1}{m}\sum_{i=1}^{m}\left[y^{(i)}\mathrm{log}\left(f_{\overrightarrow{w},b}(\vec{x}^{(i)})\right)+(1-y^{(i)})\mathrm{log}\left(1-f_{\overrightarrow{w},b}(\vec{x}^{(i)})\right)\right]$

<img src="G:\ML_learning\2022-Machine-Learning-Specialization-main\Supervised Machine Learning Regression and Classification\week3\QQ截图20240430210434.png" style="zoom: 50%;" />



<img src="G:\ML_learning\2022-Machine-Learning-Specialization-main\Supervised Machine Learning Regression and Classification\week3\QQ截图20240430210614.png" style="zoom: 50%;" />

L对w求导的大致过程为：

<img src="G:\ML_learning\2022-Machine-Learning-Specialization-main\Supervised Machine Learning Regression and Classification\week3\QQ截图20240430220948.png" style="zoom:67%;" />

更新$w_j$和$b$:
$$\begin{align*}
& \; \lbrace \\
&  \; \; \;w_j = w_j -  \alpha \frac{\partial J(\mathbf{w},b)}{\partial w_j} \\ 
&  \; \; \;  \; \;b = b -  \alpha \frac{\partial J(\mathbf{w},b)}{\partial b} \\
&\rbrace
\end{align*}$$

其中，每次迭代时同步更新梯度值：
$$\begin{align*}
\frac{\partial J(\mathbf{w},b)}{\partial w_j}  &= \frac{1}{m} \sum\limits_{i = 0}^{m-1} (f_{\mathbf{w},b}(\mathbf{x}^{(i)}) - y^{(i)})x_{j}^{(i)} \tag{2} \\
\frac{\partial J(\mathbf{w},b)}{\partial b}  &= \frac{1}{m} \sum\limits_{i = 0}^{m-1} (f_{\mathbf{w},b}(\mathbf{x}^{(i)}) - y^{(i)}) \tag{3} 
\end{align*}$$

* m 是数据集中训练例子的数量
* $f_{\mathbf{w},b}(x^{(i)})$为模型预测值, 而 $y^{(i)}$ 则是目标值
* 对于一个逻辑回归模型而言  
    $z = \mathbf{w} \cdot \mathbf{x} + b$  
    $f_{\mathbf{w},b}(x) = g(z)$  
    其中 $g(z)$ 表达式为： 
    $g(z) = \frac{1}{1+e^{-z}}$

### 正则化方法（在代价函数中添加一个关于w的惩罚量，目的是避免过拟合）

#### 线性正则化

新的代价函数为：$$J(\mathbf{w},b) = \frac{1}{2m} \sum\limits_{i = 0}^{m-1} (f_{\mathbf{w},b}(\mathbf{x}^{(i)}) - y^{(i)})^2  + \frac{\lambda}{2m}  \sum_{j=0}^{n-1} w_j^2 \tag{1}$$ 
其中$$ f_{\mathbf{w},b}(\mathbf{x}^{(i)}) = \mathbf{w} \cdot \mathbf{x}^{(i)} + b  \tag{2} $$ 

#### 逻辑正则化

新的代价函数为：$$J(\mathbf{w},b) = \frac{1}{m}  \sum_{i=0}^{m-1} \left[ -y^{(i)} \log\left(f_{\mathbf{w},b}\left( \mathbf{x}^{(i)} \right) \right) - \left( 1 - y^{(i)}\right) \log \left( 1 - f_{\mathbf{w},b}\left( \mathbf{x}^{(i)} \right) \right) \right] + \frac{\lambda}{2m}  \sum_{j=0}^{n-1} w_j^2 \tag{3}$$
其中$$ f_{\mathbf{w},b}(\mathbf{x}^{(i)}) = sigmoid(\mathbf{w} \cdot \mathbf{x}^{(i)} + b)  \tag{4} $$ 



```python
import copy, math
import numpy as np
import matplotlib.pyplot as plt
#以下面数据作为训练集，目的是实现二分类
X_train = np.array([[0.5, 1.5], [1,1], [1.5, 0.5], [3, 0.5], [2, 2], [1, 2.5]])
y_train = np.array([0, 0, 0, 1, 1, 1])
def sigmoid(z):
    """
    Parameters
    z : array_like
        A scalar or numpy array of any size.
    Returns
     g : array_like
         sigmoid(z)
    """
    z = np.clip( z, -500, 500 )           # protect against overflow
    g = 1.0/(1.0+np.exp(-z))
    return g
#使用梯度下降实现，首先计算梯度
def compute_gradient_logistic(X, y, w, b): 
    """
    Args:
      X (ndarray (m,n): Data, m examples with n features
      y (ndarray (m,)): target values
      w (ndarray (n,)): model parameters  
      b (scalar)      : model parameter
    Returns
      dj_dw (ndarray (n,)): The gradient of the cost w.r.t. the parameters w. 
      dj_db (scalar)      : The gradient of the cost w.r.t. the parameter b. 
    """
    m,n = X.shape
    dj_dw = np.zeros((n,))                           #(n,)
    dj_db = 0.

    for i in range(m):
        f_wb_i = sigmoid(np.dot(X[i],w) + b)          #(n,)(n,)=scalar
        err_i  = f_wb_i  - y[i]                       #scalar
        for j in range(n):
            dj_dw[j] = dj_dw[j] + err_i * X[i,j]      #scalar
        dj_db = dj_db + err_i
    dj_dw = dj_dw/m                                   #(n,)
    dj_db = dj_db/m                                   #scalar
        
    return dj_db, dj_dw

#逻辑回归代价函数
def compute_cost_logistic(X, y, w, b, lambda_=0, safe=False):
    """
    Args:
      X (ndarray): Shape (m,n)  matrix of examples with n features
      y (ndarray): Shape (m,)   target values
      w (ndarray): Shape (n,)   parameters for prediction
      b (scalar):               parameter  for prediction
      lambda_ : (scalar, float) Controls amount of regularization, 0 = no regularization
      safe : (boolean)          True-selects under/overflow safe algorithm
    Returns:
      cost (scalar): cost
    """
    m,n = X.shape
    cost = 0.0
    for i in range(m):
        z_i    = np.dot(X[i],w) + b                                           
        f_wb_i = sigmoid(z_i)                                                 
        cost  += -y[i] * np.log(f_wb_i) - (1 - y[i]) * np.log(1 - f_wb_i)         # scalar
    cost = cost/m
    reg_cost = 0
    if lambda_ != 0:
        for j in range(n):
            reg_cost += (w[j]**2)                                               # scalar
        reg_cost = (lambda_/(2*m))*reg_cost

    return cost + reg_cost

def gradient_descent(X, y, w_in, b_in, alpha, num_iters): 
    """
    Args:
      X (ndarray (m,n)   : Data, m examples with n features
      y (ndarray (m,))   : target values
      w_in (ndarray (n,)): Initial values of model parameters  
      b_in (scalar)      : Initial values of model parameter
      alpha (float)      : Learning rate
      num_iters (scalar) : number of iterations to run gradient descent  
    Returns:
      w (ndarray (n,))   : Updated values of parameters
      b (scalar)         : Updated value of parameter 
    """
    # An array to store cost J and w's at each iteration primarily for graphing later
    J_history = []
    w = copy.deepcopy(w_in)  #avoid modifying global w within function
    b = b_in
    
    for i in range(num_iters):
        # Calculate the gradient and update the parameters
        dj_db, dj_dw = compute_gradient_logistic(X, y, w, b)   

        # Update Parameters using w, b, alpha and gradient
        w = w - alpha * dj_dw               
        b = b - alpha * dj_db               
      
        # Save cost J at each iteration
        if i<100000:      # prevent resource exhaustion 
            J_history.append( compute_cost_logistic(X, y, w, b) )

        # Print cost every at intervals 10 times or as many iterations if < 10
        if i% math.ceil(num_iters / 10) == 0:
            print(f"Iteration {i:4d}: Cost {J_history[-1]}   ")
        
    return w, b, J_history         #return final w,b and J history for graphing

#训练获得w和b
w_tmp  = np.zeros_like(X_train[0])
b_tmp  = 0.
alph = 0.1
iters = 10000
w_out, b_out, _ = gradient_descent(X_train, y_train, w_tmp, b_tmp, alph, iters) 

def plot_data(X, y, ax, pos_label="y=1", neg_label="y=0", s=80, loc='best' ):
    """ plots logistic data with two axis """
    # Find Indices of Positive and Negative Examples
    pos = y == 1
    neg = y == 0
    pos = pos.reshape(-1,)  #work with 1D or 1D y vectors
    neg = neg.reshape(-1,)

    # Plot examples
    ax.scatter(X[pos, 0], X[pos, 1], marker='x', s=s, c = 'red', label=pos_label)
    ax.scatter(X[neg, 0], X[neg, 1], marker='o', s=s, label=neg_label, facecolors='none', edgecolors=dlblue, lw=3)
    ax.legend(loc=loc)
    ax.figure.canvas.toolbar_visible = False
    ax.figure.canvas.header_visible = False
    ax.figure.canvas.footer_visible = False
#以下为可视化，其中直线为决策边界
fig,ax = plt.subplots(1,1,figsize=(5,4))

# Plot the original data
ax.set_ylabel(r'$x_1$')
ax.set_xlabel(r'$x_0$')   
ax.axis([0, 4, 0, 3.5])
plot_data(X_train,y_train,ax)

# Plot the decision boundary
x0 = -b_out/w_out[1]
x1 = -b_out/w_out[0]
ax.plot([0,x0],[x1,0], c=dlc["dlblue"], lw=1)
plt.show()
```

运行结果：

Iteration    0: Cost 0.684610468560574   
Iteration 1000: Cost 0.1590977666870457   
Iteration 2000: Cost 0.08460064176930078   
Iteration 3000: Cost 0.05705327279402531   
Iteration 4000: Cost 0.04290759421682   
Iteration 5000: Cost 0.03433847729884557   
Iteration 6000: Cost 0.02860379802212006   
Iteration 7000: Cost 0.02450156960879306   
Iteration 8000: Cost 0.02142370332569295   
Iteration 9000: Cost 0.019030137124109114

 ![](G:\ML_learning\2022-Machine-Learning-Specialization-main\Supervised Machine Learning Regression and Classification\week3\Figure_1.svg)
