--XX－セイバー ガルセム
function c77330185.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(77330185,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c77330185.condition)
	e1:SetTarget(c77330185.target)
	e1:SetOperation(c77330185.operation)
	c:RegisterEffect(e1)
	--atk
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetValue(c77330185.atkval)
	c:RegisterEffect(e2)
end
function c77330185.atfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x100d)
end
function c77330185.atkval(e,c)
	return Duel.GetMatchingGroupCount(c77330185.atfilter,c:GetControler(),LOCATION_MZONE,0,nil)*200
end
function c77330185.condition(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetReason(),0x41)==0x41
		and e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
end
function c77330185.filter(c)
	return c:IsSetCard(0x100d) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c77330185.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c77330185.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c77330185.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c77330185.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
