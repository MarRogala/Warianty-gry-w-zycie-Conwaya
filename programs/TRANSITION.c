skipState(0, 1.0);
cond1 = count(0, 1.0) > 0;
if(cond1 && (random(0.0, 1.0) > 0.5))
{
    newState[0] = 1;
}
if(cond1)
{
    newState[1] = state[1] + 1;
}
