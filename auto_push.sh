#!/bin/bash

# �Զ���ȡ���´���
git pull

# ������и���
git add .

# �ύ�������Զ�ʱ���
git commit -m "Auto update: $(date '+%Y-%m-%d %H:%M:%S')"

# ���͵�Զ�ֿ̲�
git push
