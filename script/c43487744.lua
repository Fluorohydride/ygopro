--ホワイト·ホール
function c43487744.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c43487744.condition)
	e1:SetOperation(c43487744.activate)
	c:RegisterEffect(e1)
end
function c43487744.condition(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:GetHandler():IsCode(53129443)
end
function c43487744.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetValue(c43487744.indval)
	e1:SetReset(RESET_CHAIN)
	e1:SetLabel(ev)
	Duel.RegisterEffect(e1,tp)
end
function c43487744.indval(e,re,rp)
	return Duel.GetCurrentChain()==e:GetLabel()
end
