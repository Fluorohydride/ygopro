--キングゴブリン
function c18590133.initial_effect(c)
	--cannot be battle target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetCondition(c18590133.ccon)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
	--atk,def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCode(EFFECT_SET_BASE_ATTACK)
	e2:SetValue(c18590133.val)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_SET_BASE_DEFENCE)
	c:RegisterEffect(e3)
end
function c18590133.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_FIEND)
end
function c18590133.ccon(e)
	return Duel.IsExistingMatchingCard(c18590133.filter,e:GetHandler():GetControler(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c18590133.val(e,c)
	return Duel.GetMatchingGroupCount(c18590133.filter,c:GetControler(),LOCATION_MZONE,LOCATION_MZONE,c)*1000
end
