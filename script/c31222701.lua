--揺れる眼差し
function c31222701.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c31222701.target)
	e1:SetOperation(c31222701.activate)
	c:RegisterEffect(e1)
end
function c31222701.filter(c)
	return (c:GetSequence()==6 or c:GetSequence()==7) and c:IsDestructable()
end
function c31222701.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c31222701.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c31222701.filter,tp,LOCATION_SZONE,LOCATION_SZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c31222701.thfilter1(c)
	return c:IsType(TYPE_PENDULUM) and c:IsAbleToHand()
end
function c31222701.thfilter2(c)
	return c:IsCode(31222701) and c:IsAbleToHand()
end
function c31222701.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c31222701.filter,tp,LOCATION_SZONE,LOCATION_SZONE,nil)
	local ct=Duel.Destroy(g,REASON_EFFECT)
	if ct>=1 then
		Duel.BreakEffect()
		Duel.Damage(1-tp,500,REASON_EFFECT)
	end
	local hg1=Duel.GetMatchingGroup(c31222701.thfilter1,tp,LOCATION_DECK,0,nil)
	if ct>=2 and hg1:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(31222701,0)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local shg1=hg1:Select(tp,1,1,nil)
		Duel.SendtoHand(shg1,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,shg1)
	end
	local rg=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
	if ct>=3 and rg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(31222701,1)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local srg=rg:Select(tp,1,1,nil)
		Duel.Remove(srg,POS_FACEUP,REASON_EFFECT)
	end
	local hg2=Duel.GetMatchingGroup(c31222701.thfilter2,tp,LOCATION_DECK,0,nil)
	if ct==4 and hg2:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(31222701,2)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local shg2=hg2:Select(tp,1,1,nil)
		Duel.SendtoHand(shg2,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,shg2)
	end
end
