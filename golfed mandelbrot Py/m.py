from PIL import Image
h,_,*c=500,range;e=h*2;s=5/e
for j in _(10**6):
 i,r=(j//e-h)*s,(j%e-h)*s;q=w=z=0
 for k in _(55):
  b,n=q*q,w*w
  if 4<b+n:z=k;break
  q,w=b-n+r,2*q*w+i
 c+=(z*5,z*2,0),
g=Image.new("RGB",[e]*2);g.putdata(c);g.save("m.png")