--剣の采配
function c12197543.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_HANDES+CATEGORY_DESTROY)
	e1:SetCode(EVENT_DRAW)
	e1:SetCondition(c12197543.condition)
	e1:SetOperation(c12197543.activate)
	c:RegisterEffect(e1)
end
function c12197543.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and r==REASON_RULE
end
function c12197543.dfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c12197543.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc:IsLocation(LOCATION_HAND) then
		Duel.ConfirmCards(tp,tc)
		if tc:IsType(TYPE_SPELL+TYPE_TRAP) then
			local g=Duel.GetMatchingGroup(c12197543.dfilter,tp,0,LOCATION_ONFIELD,nil)
			local opt=0
			if g:GetCount()==0 then
				opt=Duel.SelectOption(tp,aux.Stringid(12197543,0))
			else
				opt=Duel.SelectOption(tp,aux.Stringid(12197543,0),aux.Stringid(12197543,1))
			end
			if opt==0 then Duel.SendtoGrave(tc,REASON_EFFECT+REASON_DISCARD)
			else
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
				local dg=g:Select(tp,1,1,nil)
				Duel.Destroy(dg,REASON_EFFECT)
				Duel.ShuffleHand(1-tp)
			end
		end
	else
		Duel.ShuffleHand(1-tp)
	end
end
