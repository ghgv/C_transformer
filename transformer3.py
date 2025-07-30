import math, random

# -------- utilidades --------
def matmul(A, B):
    m, n = len(A), len(A[0])
    n2, p = len(B), len(B[0])
    assert n == n2
    return [[sum(A[i][k]*B[k][j] for k in range(n)) for j in range(p)] for i in range(m)]

def transpose(M):
    return list(map(list, zip(*M)))

def softmax_rowwise(S):
    out = []
    for row in S:
        m = max(row)
        exps = [math.exp(x - m) if x > -1e8 else 0.0 for x in row]  # exp(-inf)=0
        s = sum(exps)
        out.append([x/s if s > 0 else 0.0 for x in exps])
    return out

def mse_loss(Y, T):
    n, d = len(Y), len(Y[0])
    diff = [[Y[i][j] - T[i][j] for j in range(d)] for i in range(n)]
    loss = sum(sum(x*x for x in row) for row in diff)/(n*d)
    return loss, diff

# -------- GELU --------
def gelu(M):
    out = []
    for row in M:
        new_row = []
        for x in row:
            c = math.sqrt(2.0 / math.pi)
            t = c * (x + 0.044715 * x**3)
            new_row.append(0.5 * x * (1 + math.tanh(t)))
        out.append(new_row)
    return out

def dgelu(M):
    out = []
    for row in M:
        new_row = []
        for x in row:
            c = math.sqrt(2.0 / math.pi)
            t = c * (x + 0.044715 * x**3)
            th = math.tanh(t)
            sech2 = 1 - th**2
            d = 0.5 * (1 + th) + 0.5 * x * sech2 * c * (1 + 3*0.044715*x*x)
            new_row.append(d)
        out.append(new_row)
    return out

# -------- forward con FF + GELU + máscara causal --------
def forward_with_ff(X, WQ, WK, WV, W_out, W1, W2, num_heads):
    seq_len, d_model = len(X), len(X[0])
    d_k = d_model // num_heads

    Q = matmul(X, WQ)
    K = matmul(X, WK)
    V = matmul(X, WV)

    heads_out, caches = [], []
    for h in range(num_heads):
        qh = [q[h*d_k:(h+1)*d_k] for q in Q]
        kh = [k[h*d_k:(h+1)*d_k] for k in K]
        vh = [v[h*d_k:(h+1)*d_k] for v in V]

        scale = 1.0/math.sqrt(d_k)
        S = [[sum(qh[i][kk]*kh[j][kk] for kk in range(d_k))*scale
              for j in range(seq_len)] for i in range(seq_len)]

        # máscara causal
        for i in range(seq_len):
            for j in range(seq_len):
                if j > i:
                    S[i][j] = -1e9

        A = softmax_rowwise(S)

        out = []
        for i in range(seq_len):
            vec = [0.0]*d_k
            for j in range(seq_len):
                for kk in range(d_k):
                    vec[kk] += A[i][j]*vh[j][kk]
            out.append(vec)
        heads_out.append(out)
        caches.append((qh, kh, vh, S, A))

    Z = []
    for i in range(seq_len):
        concat = []
        for h in range(num_heads):
            concat.extend(heads_out[h][i])
        Z.append(concat)

    Y = matmul(Z, W_out)
    H = matmul(Y, W1)
    H_gelu = gelu(H)
    Y_ff = matmul(H_gelu, W2)

    cache = (X, Q, K, V, caches, Z, Y, H, H_gelu)
    return Y_ff, cache

# -------- backward --------
def backward_with_ff(dYff, cache, WQ, WK, WV, W_out, W1, W2, num_heads):
    X, Q, K, V, caches, Z, Y, H, H_gelu = cache
    seq_len, d_model = len(X), len(X[0])
    d_ff = len(W1[0])
    d_k = d_model // num_heads

    dW2 = matmul(transpose(H_gelu), dYff)
    dH_gelu = matmul(dYff, transpose(W2))
    dH = [[dH_gelu[i][j]*dgelu(H)[i][j] for j in range(d_ff)] for i in range(seq_len)]
    dW1 = matmul(transpose(Y), dH)
    dY = matmul(dH, transpose(W1))

    dW_out = matmul(transpose(Z), dY)
    dZ = matmul(dY, transpose(W_out))

    dQ_total = [[0.0]*d_model for _ in range(seq_len)]
    dK_total = [[0.0]*d_model for _ in range(seq_len)]
    dV_total = [[0.0]*d_model for _ in range(seq_len)]

    for h in range(num_heads):
        qh, kh, vh, S, A = caches[h]
        dZ_head = [dz[h*d_k:(h+1)*d_k] for dz in dZ]

        dVh = matmul(transpose(A), dZ_head)
        dA = matmul(dZ_head, transpose(vh))

        dS = []
        for i in range(seq_len):
            a = A[i]; da = dA[i]
            J = [[(1 if j==k else 0)*a[j] - a[j]*a[k] for k in range(seq_len)] for j in range(seq_len)]
            dS_row = [sum(J[j][k]*da[k] for k in range(seq_len)) for j in range(seq_len)]
            dS.append(dS_row)

        scale = 1.0/math.sqrt(d_k)
        dS = [[val*scale for val in row] for row in dS]

        dQh = matmul(dS, kh)
        dKh = matmul(transpose(dS), qh)

        for i in range(seq_len):
            for kk in range(d_k):
                dQ_total[i][h*d_k+kk] += dQh[i][kk]
                dK_total[i][h*d_k+kk] += dKh[i][kk]
                dV_total[i][h*d_k+kk] += dVh[i][kk]

    dWQ = matmul(transpose(X), dQ_total)
    dWK = matmul(transpose(X), dK_total)
    dWV = matmul(transpose(X), dV_total)

    return dWQ, dWK, dWV, dW_out, dW1, dW2

# -------- inferencia --------
def infer(context, WQ, WK, WV, W_out, W1, W2, num_heads, steps=3):
    seq = [row[:] for row in context]
    for s in range(steps):
        Y_ff, _ = forward_with_ff(seq, WQ, WK, WV, W_out, W1, W2, num_heads)
        next_vec = Y_ff[-1]
        token = [round(x, 3) for x in next_vec]
        print(f"Step {s}: pred -> {token}")
        seq.append(next_vec)
    return seq

# -------- demo --------
if __name__ == "__main__":
    seq_len, d_model, num_heads = 3, 4, 2
    d_ff = 8

    X = [[0.1,0.2,0.3,0.4],
         [0.2,0.1,0.0,0.1],
         [0.5,0.6,0.7,0.8]]

    T = [[0.2,0.1,0.0,0.1],
         [0.5,0.6,0.7,0.8],
         [0.9,1.0,1.1,1.2]]

    random.seed(0)
    WQ = [[random.uniform(-1,1) for _ in range(d_model)] for _ in range(d_model)]
    WK = [[random.uniform(-1,1) for _ in range(d_model)] for _ in range(d_model)]
    WV = [[random.uniform(-1,1) for _ in range(d_model)] for _ in range(d_model)]
    W_out = [[random.uniform(-1,1) for _ in range(d_model)] for _ in range(d_model)]
    W1 = [[random.uniform(-1,1) for _ in range(d_ff)] for _ in range(d_model)]
    W2 = [[random.uniform(-1,1) for _ in range(d_model)] for _ in range(d_ff)]

    lr = 0.05
    for epoch in range(3720):
        Y_ff, cache = forward_with_ff(X, WQ, WK, WV, W_out, W1, W2, num_heads)
        loss, diff = mse_loss(Y_ff, T)
        dWQ, dWK, dWV, dW_out, dW1, dW2 = backward_with_ff(diff, cache, WQ, WK, WV, W_out, W1, W2, num_heads)
        for i in range(d_model):
            for j in range(d_model):
                WQ[i][j] -= lr*dWQ[i][j]
                WK[i][j] -= lr*dWK[i][j]
                WV[i][j] -= lr*dWV[i][j]
                W_out[i][j] -= lr*dW_out[i][j]
        for i in range(d_model):
            for j in range(d_ff):
                W1[i][j] -= lr*dW1[i][j]
        for i in range(d_ff):
            for j in range(d_model):
                W2[i][j] -= lr*dW2[i][j]
        print(f"Epoch {epoch}, Loss={loss:.6f}")

    print("\n--- Inference ---")
    context = [[0.1,0.2,0.3,0.4]]
    seq = infer(context, WQ, WK, WV, W_out, W1, W2, num_heads, steps=3)
