# exp1原理清楚，自己完成逻辑代码书写，AI辅助完成重复性代码编写，README 整理思路

## 1. 数据定义（测试流）
- `a[] = {1,0,1,1,0}`: 信号 A（5 比特）
- `b[] = {0,1,1,0}`: 信号 B（4 比特）
- `c[256]`：复用输出（共享信道）  
- `da/db`：解复用恢复结果  

---

## 2. 同步时分多路复用 `multiplex()` / `demultiplex()`
### `multiplex`
- 选择 `n = max(a_len,b_len)`（5）
- 输出长度 `2*n`（10），固定时隙（A,B交替）  
- `c[0]=a0, c[1]=b0, c[2]=a1, c[3]=b1...`  
- 如果某流短（B在末尾），赋 0 填充（`i<b_len` 条件）

### `demultiplex`
- 要求 `c_len` 偶数
- 拆出 `a[i]=c[2*i]`, `b[i]=c[2*i+1]`
- `n=c_len/2` 都取到

> 这是最经典“同步TDM”：每个通道占固定时隙，长度固定，容易解复用。

---

## 3. 统计时分复用 `multiplex_statistical()` / `demultiplex_statistical()`
### `multiplex_statistical`
- 顺序追加 A 后 B（无空时隙，紧凑）
- 先写完 A 再写 B
- 返回真实长度 `a_len+b_len`（如果 c_size 足够）

### `demultiplex_statistical`
- 你在解复用里直接“已知 a_len,b_len”：
  - `a[i]=c[i]`（前段）
  - `b[i]=c[a_len+i]`（后段）
- 返回 `max(a_len,b_len)`（这个有点奇怪但不影响演示）

> 这个是“统计TDM”思想：根据占用数据实际发送，无固定帧占位，和协议层“报文打包”类似。

---

## 4. 频分多路复用 `multiplex_frequency()` / `demultiplex_frequency()`
### `multiplex_frequency`
- 对 `i` 位置:
  - `a=1<<0` (b0 位)
  - `b=2<<0` (b1 位)
- `c[i] = (a_bit?1:0) | (b_bit?2:0)`
- 所以一个符号含2路：bit0代表A，bit1代表B

### `demultiplex_frequency`
- 反向
  - `a[i] = c[i] & 1 ? 1:0`
  - `b[i] = c[i] & 2 ? 1:0`

> 这里用位域模拟“频分”：不同信号放到同一时间位置不同“子载波”（比特位）。

---

## 5. 码分多路复用 `multiplex_code()` / `demultiplex_code()`
### `multiplex_code`
- 每个时刻 `i` 变成2个芯片（2个数组单元）
- a信号的 “码片” 直接放 `c[2*i]`
- b信号的 “码片” 直接放 `c[2*i+1]`
- 其实是等价同步 TDM 但称CDM：用不同“码槽”

### `demultiplex_code`
- 同样按位置取回  
- `a[i]=c[2*i], b[i]=c[2*i+1]`

---

## 6. 结果打印
- `printSeq(...)`：格式化输出数组内容  
- `main` 依次跑4种复用/解复用并打印:
  - `c` 合成后的复用块
  - `da/db` 解复用恢复值

---

# exp2实在无法理解，只能all in AI 实现，等有空了再尝试理解
