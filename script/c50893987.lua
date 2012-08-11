--剣闘獣ティゲル
function c50893987.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(50893987,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c50893987.sccon)
	e1:SetCost(c50893987.sccost)
	e1:SetTarget(c50893987.sctg)
	e1:SetOperation(c50893987.scop)
	c:RegisterEffect(e1)
	--fusion limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_BE_FUSION_MATERIAL)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetValue(1)
	c:RegisterEffect(e2)
end
function c50893987.sccon(e,tp,eg,ep,ev,re,r,rp)
	local st=e:GetHandler():GetSummonType()
	return st>=(SUMMON_TYPE_SPECIAL+100) and st<(SUMMON_TYPE_SPECIAL+150)
end
function c50893987.costfilter(c)
	return c:IsSetCard(0x19) and c:IsDiscardable()
end
function c50893987.sccost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c50893987.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c50893987.costfilter,1,1,REASON_DISCARD+REASON_COST,nil)
end
function c50893987.scfilter(c)
	return c:IsSetCard(0x19) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c50893987.sctg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c50893987.scfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c50893987.scop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c50893987.scfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
