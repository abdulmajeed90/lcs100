
ֻ�ܿ�����������ʾ����ִ��
for /R .\ %f in (*.h) do astyle --style=allman "%f"
for /R .\ %f in (*.c) do astyle --style=allman "%f"
rm **/*.orig