--古代の機械箱
function c60953949.initial_effect(c)
	--tohand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60953949,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetCountLimit(1,60953949)
	e1:SetCondition(c60953949.thcon)
	e1:SetTarget(c60953949.thtg)
	e1:SetOperation(c60953949.thop)
	c:RegisterEffect(e1)
end
function c60953949.thcon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsReason(REASON_DRAW)
		and e:GetHandler():IsPreviousLocation(LOCATION_DECK+LOCATION_GRAVE)
end
function c60953949.filter(c)
	return (c:GetAttack()==500 or c:GetDefence()==500) and c:GetCode()~=60953949
		and c:IsRace(RACE_MACHINE) and c:IsAttribute(ATTRIBUTE_EARTH) and c:IsAbleToHand()
end
function c60953949.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c60953949.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c60953949.thop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c60953949.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
