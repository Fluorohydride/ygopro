--死のデッキ破壊ウイルス
function c57728570.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_TOHAND)
	e1:SetCost(c57728570.cost)
	e1:SetTarget(c57728570.target)
	e1:SetOperation(c57728570.activate)
	c:RegisterEffect(e1)
end
function c57728570.costfilter(c)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:IsAttackBelow(1000)
end
function c57728570.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c57728570.costfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c57728570.costfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c57728570.filter(c)
	return c:IsAttackAbove(1500) and c:IsDestructable()
end
function c57728570.hgfilter(c)
	return not c:IsPublic() or c57728570.filter(c)
end
function c57728570.fgfilter(c)
	return c:IsFacedown() or c57728570.filter(c)
end
function c57728570.tgfilter(c)
	return ((c:IsLocation(LOCATION_HAND) and c:IsPublic()) or (c:IsLocation(LOCATION_MZONE) and c:IsFaceup())) and c57728570.filter(c)
end
function c57728570.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c57728570.hgfilter,tp,0,LOCATION_HAND,1,nil)
		or Duel.IsExistingMatchingCard(c57728570.fgfilter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c57728570.tgfilter,tp,0,LOCATION_MZONE+LOCATION_HAND,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c57728570.activate(e,tp,eg,ep,ev,re,r,rp)
	local conf=Duel.GetFieldGroup(tp,0,LOCATION_MZONE+LOCATION_HAND)
	local ct=0
	if conf:GetCount()>0 then
		Duel.ConfirmCards(tp,conf)
		local dg=conf:Filter(c57728570.filter,nil)
		ct=Duel.Destroy(dg,REASON_EFFECT)
		Duel.ShuffleHand(1-tp)
	end
	local g=Duel.GetMatchingGroup(c57728570.filter,1-tp,LOCATION_DECK,0,nil)
	if ct>0 and g:GetCount()>0 and Duel.SelectYesNo(1-tp,aux.Stringid(57728570,0)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_DESTROY)
		local dg=g:Select(1-tp,1,3,nil)
		Duel.Destroy(dg,REASON_EFFECT)
	end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	e1:SetValue(0)
	e1:SetReset(RESET_PHASE+PHASE_END,2)
	Duel.RegisterEffect(e1,tp)
end
