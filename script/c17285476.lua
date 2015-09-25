--ナチュル・モスキート
function c17285476.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetCondition(c17285476.atcon)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
	--reflect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_REFLECT_BATTLE_DAMAGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c17285476.reftg)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
function c17285476.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x2a)
end
function c17285476.atcon(e)
	return Duel.IsExistingMatchingCard(c17285476.cfilter,e:GetOwnerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c17285476.reftg(e,c)
	return c~=e:GetHandler() and c:IsFaceup() and c:IsSetCard(0x2a)
end
