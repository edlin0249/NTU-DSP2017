import sys

#fin = open(sys.argv[1], 'r', encoding='BIG5-HKSCS')
#fout = open(sys.argv[2], 'w', encoding='BIG5-HKSCS')

with open(sys.argv[1], 'r', encoding='BIG5-HKSCS') as fin:
	dic = {'ㄅ':[], 'ㄆ':[], 'ㄇ':[], 'ㄈ':[], 'ㄉ':[], 'ㄊ':[], 'ㄋ':[], 'ㄌ':[], 'ㄍ':[], 'ㄎ':[], 'ㄏ':[], 'ㄐ':[], 'ㄑ':[], 'ㄒ':[], 'ㄓ':[], 'ㄔ':[], 'ㄕ':[], 'ㄖ':[], 'ㄗ':[], 'ㄘ':[], 'ㄙ':[], 'ㄧ':[], 'ㄨ':[], 'ㄩ':[], 'ㄚ':[], 'ㄛ':[], 'ㄜ':[], 'ㄝ':[], 'ㄞ':[], 'ㄟ':[], 'ㄠ':[], 'ㄡ':[], 'ㄢ':[], 'ㄣ':[], 'ㄤ':[], 'ㄥ':[], 'ㄦ':[]}

	for r in fin:
		t = r.split()
		#x.append(t[1])
		t[1] = t[1].split('/')
		for e in t[1]:
			for j in e:
				if j in dic:
					dic[j].append(t[0])
		dic[t[0]] = t[0]
	fin.close()
	#print(x)
	with open(sys.argv[2], 'w', encoding='BIG5-HKSCS') as fout:
		for k in sorted(dic.keys()):
			fout.write(k+' '+' '.join(dic[k])+'\n')
		fout.close()