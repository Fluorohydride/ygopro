--氷結界の守護陣
function c82498947.initial_effect(c)
	--cannot attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetTarget(c82498947.tg)
	e1:SetCondition(c82498947.con)
	c:RegisterEffect(e1)
end
function c82498947.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x2f)
end
function c82498947.con(e)
	return Duel.IsExistingMatchingCard(c82498947.filter,e:GetHandler():GetControler(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c82498947.tg(e,c)
	return c:GetAttack()>=e:GetHandler():GetDefence()
end
