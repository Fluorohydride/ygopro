--森羅の姫芽君 スプラウト
function c20579538.initial_effect(c)
	--deck check
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(20579538,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,20579538)
	e1:SetCost(c20579538.cost)
	e1:SetTarget(c20579538.target)
	e1:SetOperation(c20579538.operation)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(20579538,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DELAY)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCountLimit(1,20579539)
	e2:SetCondition(c20579538.spcon)
	e2:SetTarget(c20579538.sptg)
	e2:SetOperation(c20579538.spop)
	c:RegisterEffect(e2)
end
function c20579538.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c20579538.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(tp,1) end
end
function c20579538.tdfilter(c)
	return c:IsSetCard(0xa6) and c:IsType(TYPE_MONSTER) and c:IsAbleToDeck()
end
function c20579538.operation(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsPlayerCanDiscardDeck(tp,1) then return end
	Duel.ConfirmDecktop(tp,1)
	local g=Duel.GetDecktopGroup(tp,1)
	Duel.DisableShuffleCheck()
	if Duel.SendtoGrave(g,REASON_EFFECT+REASON_REVEAL)==0 then return end
	local dg=Duel.GetMatchingGroup(c20579538.tdfilter,tp,LOCATION_GRAVE,0,nil)
	if dg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(20579538,2)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local tg=dg:Select(tp,1,1,nil)
		Duel.HintSelection(tg)
		Duel.SendtoDeck(tg,nil,0,REASON_EFFECT)
	end
end
function c20579538.spcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsPreviousLocation(LOCATION_DECK) and
		(c:IsReason(REASON_REVEAL) or c:GetPreviousPosition()==POS_FACEUP_DEFENCE or Duel.IsPlayerAffectedByEffect(tp,EFFECT_REVERSE_DECK))
end
function c20579538.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	local t={}
	local i=1
	for i=1,8 do t[i]=i end
	Duel.Hint(HINT_SELECTMSG,tp,567)
	local lv=Duel.AnnounceNumber(tp,table.unpack(t))
	e:SetLabel(lv)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c20579538.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LEVEL)
		e1:SetValue(e:GetLabel())
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
end
