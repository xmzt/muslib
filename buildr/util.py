#------------------------------------------------------------------------------------------------------------------------
# helpers
#------------------------------------------------------------------------------------------------------------------------

def dump1(pre, obj):
    if isinstance(obj, list):
        print(f'{pre}: list')
        for v in obj:
            print(f'    {v!r}')
    elif isinstance(obj, (dict,Extension)):
        print(f'{pre}: dict')
        for k in dir(obj):
            print(f'    {k!r}: {getattr(obj, k)!r}')
    else:
        print(f'{pre}: {type(obj)} {obj!r}')

