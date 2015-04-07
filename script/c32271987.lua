--フェデライザー
function c32271987.initial_effect(c)
	--tograve & draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(32271987,0))
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c32271987.condition)
	e1:SetTarget(c32271987.target)
	e1:SetOperation(c32271987.operation)
	c:RegisterEffect(e1)
end
function c32271987.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():IsReason(REASON_BATTLE)
end
function c32271987.filter(c)
	return c:IsType(TYPE_DUAL) and c:IsAbleToGrave()
end
function c32271987.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local ct=Duel.GetMatchingGroupCount(c32271987.filter,tp,LOCATION_DECK,0,nil)
		if ct==1 and Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==1 then return false end
		return Duel.IsPlayerCanDraw(tp,1) and ct>=1 end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c32271987.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c32271987.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
		Duel.BreakEffect()
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
