--一点買い
function c68661341.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c68661341.cost)
	e1:SetTarget(c68661341.target)
	e1:SetOperation(c68661341.activate)
	c:RegisterEffect(e1)
end
function c68661341.cfilter(c)
	return c:IsType(TYPE_MONSTER) or not c:IsAbleToRemoveAsCost()
end
function c68661341.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	g:RemoveCard(e:GetHandler())
	if chk==0 then return g:GetCount()>=3 and not g:IsExists(c68661341.cfilter,1,nil) end
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c68661341.filter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c68661341.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c68661341.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c68661341.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c68661341.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetCode(EFFECT_CANNOT_SUMMON)
		e1:SetReset(RESET_PHASE+PHASE_END)
		e1:SetTargetRange(1,0)
		e1:SetTarget(c68661341.sumlimit)
		e1:SetLabel(g:GetFirst():GetCode())
		Duel.RegisterEffect(e1,tp)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
		Duel.RegisterEffect(e2,tp)
	end
end
function c68661341.sumlimit(e,c)
	return c:GetCode()~=e:GetLabel()
end
