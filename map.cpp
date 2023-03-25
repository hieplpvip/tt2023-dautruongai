#include "testlib.h"
#include <bits/stdc++.h>

using namespace std;

// Tạo ngẫu nhiên một cái bản đồ kích thước n x m (n lẻ, m lẻ), mỗi ô có thể chứa vật cản ('D'), có chính xác 2 ô chứa khiên ('S'), hoặc số lượng vàng ('0' - '5')
// Bản đồ sẽ đối xứng qua trục x/2 hoặc y/2 hoặc trục chéo nếu x == y.

int n, m;
char a[31][31];

void symmetrize()
{
    // Random đối xứng, ngang, dọc, chéo 1, chéo 2
    int r;
    if (n == m) r = rnd.next(0, 3);
    else r = rnd.next(0, 1);
    if (r == 0)
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                a[i][j] = a[n - i - 1][j];
    }
    else if (r == 1)
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                a[i][j] = a[i][m - j - 1];
    }
    else if (r == 2)
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                a[i][j] = a[j][i];
    }
    else
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                a[i][j] = a[m - j - 1][n - i - 1];
    }
}

void genRaw()
{
    // Tạo ngẫu nhiên một bản đồ không đối xứng, mỗi ô có tỉ lệ 1/3 là 'D' và 2/3 là '0' - '5'
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i][j] = ((rnd.next(0, 9) < 3) ? 'D' : ('0' + rnd.next(0, 5)));

    // ô chính giữa = 0
    a[n / 2][m / 2] = '0';

    // chính xác 2 ô chứa khiên
    int x = rnd.next(0, n - 1), y = rnd.next(0, m - 1);
    a[x][y] = 'S';
    x = rnd.next(0, n - 1), y = rnd.next(0, m - 1);
    a[x][y] = 'S';
}

bool isConnected()
{
    // Kiểm tra rằng mỗi ô không phải là vật cản có thể đi đến tất cả những ô khác không phải vật cản
    // Nếu không thì tạo lại bản đồ
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (a[i][j] == 'D')
                continue;
            vector<vector<bool>> visited(n, vector<bool>(m, false));
            queue<pair<int, int>> q;
            q.push({i, j});
            visited[i][j] = true;
            while (!q.empty())
            {
                int x = q.front().first, y = q.front().second;
                q.pop();
                if (a[x][y] == 'D')
                    continue;
                if (x > 0 && !visited[x - 1][y])
                {
                    visited[x - 1][y] = true;
                    q.push({x - 1, y});
                }
                if (x < n - 1 && !visited[x + 1][y])
                {
                    visited[x + 1][y] = true;
                    q.push({x + 1, y});
                }
                if (y > 0 && !visited[x][y - 1])
                {
                    visited[x][y - 1] = true;
                    q.push({x, y - 1});
                }
                if (y < m - 1 && !visited[x][y + 1])
                {
                    visited[x][y + 1] = true;
                    q.push({x, y + 1});
                }
            }

            for (int ii = 0; ii < n; ii++)
                for (int jj = 0; jj < m; jj++)
                    if (a[ii][jj] != 'D' && !visited[ii][jj])
                        return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

	registerGen(argc, argv, 1);

    ofstream fout("map.txt");

    // n với m là số lẻ và nhỏ hơn hoặc bằng 15
    n = rnd.next(1, 15);
    if (n % 2 == 0)
        n++;
    m = rnd.next(1, 15);
    if (m % 2 == 0)
        m++;
    fout << n << ' ' << m << '\n';

    // TODO: tổng bằng 100
    for (int T = 1; T <= 100; T++)
    {
        genRaw();
        symmetrize();
        if (isConnected())
        {
            // Viết map ra file map.txt

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                    fout << a[i][j] << ' ';
                fout << '\n';
            }
            return 0;
        }
    }
}