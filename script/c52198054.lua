--ブレイズ・キャノン・マガジン
function c52198054.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c52198054.target1)
	e1:SetOperation(c52198054.operation)
	e1:SetHintTiming(0,TIMING_MAIN_END)
	c:RegisterEffect(e1)
	--tograve
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_TOGRAVE+CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1,52198054)
	e2:SetCondition(c52198054.condition)
	e2:SetCost(c52198054.cost)
	e2:SetTarget(c52198054.target2)
	e2:SetOperation(c52198054.operation)
	e2:SetHintTiming(0,TIMING_MAIN_END)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	--change
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetCode(EFFECT_CHANGE_CODE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetValue(21420702)
	c:RegisterEffect(e3)
	--tograve
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(52198054,1))
	e4:SetCategory(CATEGORY_TOGRAVE)
	e4:SetType(EFFECT_TYPE_QUICK_O)
	e4:SetCode(EVENT_FREE_CHAIN)
	e4:SetRange(LOCATION_GRAVE)
	e4:SetCondition(c52198054.condition)
	e4:SetCost(c52198054.tgcost)
	e4:SetTarget(c52198054.tgtg)
	e4:SetOperation(c52198054.tgop)
	e4:SetHintTiming(0,TIMING_MAIN_END)
	c:RegisterEffect(e4)
end
function c52198054.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if (Duel.GetCurrentPhase()==PHASE_MAIN1 or Duel.GetCurrentPhase()==PHASE_MAIN2)
		and Duel.GetFlagEffect(tp,52198054)==0
		and Duel.IsPlayerCanDraw(tp,1)
		and Duel.IsExistingMatchingCard(Card.IsSetCard,tp,LOCATION_HAND,0,1,nil,0x32)
		and Duel.SelectYesNo(tp,aux.Stringid(52198054,0)) then
		e:SetCategory(CATEGORY_TOGRAVE+CATEGORY_DRAW)
		e:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		Duel.RegisterFlagEffect(tp,52198054,RESET_PHASE+PHASE_END,0,1)
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_HAND)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
		e:SetLabel(1)
		e:GetHandler():RegisterFlagEffect(0,RESET_CHAIN,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(52198054,2))
	else
		e:SetCategory(0)
		e:SetProperty(0)
		e:SetLabel(0)
	end
end
function c52198054.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1 or Duel.GetCurrentPhase()==PHASE_MAIN2
end
function c52198054.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,52198054)==0 end
	Duel.RegisterFlagEffect(tp,52198054,RESET_PHASE+PHASE_END,0,1)
end
function c52198054.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1)
		and Duel.IsExistingMatchingCard(Card.IsSetCard,tp,LOCATION_HAND,0,1,nil,0x32) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_HAND)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c52198054.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,Card.IsSetCard,tp,LOCATION_HAND,0,1,1,nil,0x32)
	if g:GetCount()>0 and Duel.SendtoGrave(g,REASON_EFFECT)~=0 and g:GetFirst():IsLocation(LOCATION_GRAVE) then
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
function c52198054.tgcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c52198054.tgfilter(c)
	return c:IsSetCard(0x32) and c:IsAbleToGrave()
end
function c52198054.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c52198054.tgfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c52198054.tgop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c52198054.tgfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
