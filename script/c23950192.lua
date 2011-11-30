--氷結界の術者
function c23950192.initial_effect(c)
	--cannot attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c23950192.tg)
	e2:SetCondition(c23950192.con)
	c:RegisterEffect(e2)
end
function c23950192.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x2f)
end
function c23950192.con(e)
	return Duel.IsExistingMatchingCard(c23950192.filter,e:GetHandler():GetControler(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c23950192.tg(e,c)
	return c:GetLevel()>=4
end
