--赤竜の忍者
function c58165765.initial_effect(c)
	--todeck
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(58165765,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCountLimit(1,58165765)
	e1:SetCost(c58165765.cost)
	e1:SetTarget(c58165765.target)
	e1:SetOperation(c58165765.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e3)
end
function c58165765.cfilter(c)
	return (c:IsSetCard(0x2b) or c:IsSetCard(0x61)) and c:IsAbleToRemoveAsCost()
end
function c58165765.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c58165765.cfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c58165765.cfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c58165765.filter(c)
	return c:IsFacedown() and c:IsAbleToDeck()
end
function c58165765.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(1-tp) and chkc:IsOnField() and c58165765.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c58165765.filter,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c58165765.filter,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
	Duel.SetChainLimit(c58165765.limit(g:GetFirst()))
end
function c58165765.limit(c)
	return	function (e,lp,tp)
				return e:GetHandler()~=c
			end
end
function c58165765.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsFacedown() then
		Duel.ConfirmCards(tp,tc)
		Duel.BreakEffect()
		if tc:IsAbleToDeck() then
			local opt=Duel.SelectOption(tp,aux.Stringid(58165765,1),aux.Stringid(58165765,2))
			if opt==0 then
				Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)
			else
				Duel.SendtoDeck(tc,nil,1,REASON_EFFECT)
			end
		end
	end
end
