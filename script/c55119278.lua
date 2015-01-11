--D・ラジオン
function c55119278.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c55119278.cona)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x26))
	e1:SetValue(800)
	c:RegisterEffect(e1)
	--def
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	e2:SetCondition(c55119278.cond)
	e2:SetValue(1000)
	c:RegisterEffect(e2)
end
function c55119278.cona(e)
	return e:GetHandler():IsAttackPos()
end
function c55119278.cond(e)
	return e:GetHandler():IsDefencePos()
end
