--ナチュル・ドラゴンフライ
function c70083723.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(c70083723.indes)
	c:RegisterEffect(e1)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c70083723.atkval)
	c:RegisterEffect(e1)
end
function c70083723.indes(e,c)
	return c:GetAttack()>=2000
end
function c70083723.filter(c)
	return c:IsSetCard(0x2a) and c:IsType(TYPE_MONSTER)
end
function c70083723.atkval(e,c)
	return Duel.GetMatchingGroupCount(c70083723.filter,c:GetControler(),LOCATION_GRAVE,0,nil)*200
end
