def writeFill(f, x, y, w, r):
    f.write('      <rect x="{0}" y="{1}" width="{2}" height="{2}"/>\n'.format(x, y, w//2))
    if r > 0:
        for i in range(2):
            for k in range(2):
                if (i,k) != (0,0):
                    writeFill(f, x+i*w//2, y+k*w//2, w//2, r-1)

for i in range(6):
    f = open("karatsuba{}.svg".format(i+1), "wt")
    f.write(
"""<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 20010904//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<!--svg width="2.24cm" height="1cm" viewBox="-1 -1 23 11" xmlns="http://www.w3.org/2000/svg"-->
<svg viewBox="-1 -1 129 129" xmlns="http://www.w3.org/2000/svg">
  <g stroke="#100060" stroke-width="1">
    <g fill="#100060">
      <rect x="0" y="0" width="128" height="128" />
    </g>
    <g fill="#fff" stroke="#fff" stroke-width="0">
""")

    writeFill(f, 0, 0, 128, i)

    f.write(
    """    </g>
      </g>
    </svg>
    """)
    f.close()
