cnt = count(0, 1.0);
cond1 = cnt > 0;
rnd = random(0.0, 1.0);
cond2 = rnd > 0.75;
cond = cond1 && cond2;
if(cond)
{
    newState[0] = 1.0;
}
cond3 = cond == 0.0;
cond4 = cond1 && cond3;
if(cond4)
{
    newState[1] = state[1] + 1.0;
}
