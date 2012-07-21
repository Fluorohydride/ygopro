--義賊の入門書
function c69091732.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetHintTiming(0,TIMING_TOHAND)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c69091732.condition)
	e1:SetTarget(c69091732.target)
	e1:SetOperation(c69091732.activate)
	c:RegisterEffect(e1)
end
function c69091732.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>4
end
function c69091732.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,1-tp,1)
end
function c69091732.activate(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local g=Duel.GetFieldGroup(p,LOCATION_HAND,0)
	local dg=g:RandomSelect(tp,1)
	Duel.SendtoGrave(dg,REASON_EFFECT+REASON_DISCARD)
end
