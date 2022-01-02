-- tarantool initialization script
box.cfg {
    listen = 3301
}

box.once("bootstrap", function()
    box.schema.space.create('env')
    box.space.env:format({
        {name='repository', type='string'},
        {name='key', type='string'},
        {name='value', type='string'}
    })
    box.space.env:create_index('primary', {
        unique = true,
        parts = {
            {field=1, type='string'},
            {field=2, type='string'}
        }
    })

    box.schema.space.create('log')
    box.space.log:format({
        {name='unix', type='number'},
        {name='jobtimestamp', type='number'},
        {name='repository', type='string'},
        {name='branch', type='string'},
        {name='stage', type='string'},
        {name='text', type='string'}
    })

    box.space.log:create_index('primary', {
        unique=true,
        parts = {
            {field=3, type='string'},
            {field=2, type='number'},
            {field=1, type='number'}
        }
    })

    box.schema.space.create('crypto')
    box.space.crypto:format({
        {name='unix', type='number'},
        {name='datetime', type='string'},
        {name='symbol', type='string'},
        {name='open', type='number'},
        {name='high', type='number'},
        {name='low', type='number'},
        {name='close', type='number'},
        {name='volume_original', type='number'},
        {name='volume_usd', type='number'}
    })
    box.space.crypto:create_index('primary', {
        unique = true,
        parts = {
            {field=3, type='string'},
            {field=1, type='number'},
        }
    })

    box.schema.space.create('txn')
    box.space.txn:format({
        {name='index', type='number'},
        {name='prev_index', type='number'},
        {name='input', type='number'},
        {name='output', type='number'},
        {name='hash', type='string'}
    })
    box.space.txn:create_index('primary', {
        unique = true,
        parts = {
            {field=1, type='number'}
        }
    })

    box.schema.user.create('tnt_user', {password = 'tnt_password'})
    box.schema.user.grant('tnt_user','read,write,execute,create,drop','universe')
end)
