skip(0, 1.0);
st1 = state[0];
st2 = state[1];

if(st1)
{
    color[0] = 0.0;
    color[1] = 0.8;
    color[2] = 0.9;
}

cond1 = st2 > 4;
cond2 = st1 == 0;
cond3 = cond1 && cond2;
if(cond3)
{
    color[0] = 0.5;
    color[1] = 0.0;
    color[2] = 0.0;
}
