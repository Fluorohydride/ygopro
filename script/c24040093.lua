--魔轟神グリムロ
function c24040093.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(24040093,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c24040093.con)
	e1:SetCost(c24040093.cost)
	e1:SetTarget(c24040093.tg)
	e1:SetOperation(c24040093.op)
	c:RegisterEffect(e1)
end
function c24040093.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x35)
end
function c24040093.con(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c24040093.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c24040093.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c24040093.filter(c)
	return c:IsSetCard(0x35) and c:GetCode()~=24040093 and c:IsAbleToHand()
end
function c24040093.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c24040093.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c24040093.op(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c24040093.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
