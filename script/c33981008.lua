--魔導書院ラメイソン
function c33981008.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(33981008,0))
	e2:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCountLimit(1)
	e2:SetCondition(c33981008.drcon)
	e2:SetTarget(c33981008.drtg)
	e2:SetOperation(c33981008.drop)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(33981008,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCondition(c33981008.spcon)
	e3:SetTarget(c33981008.sptg)
	e3:SetOperation(c33981008.spop)
	c:RegisterEffect(e3)
end
function c33981008.cfilter(c)
	return c:IsRace(RACE_SPELLCASTER) and (c:IsLocation(LOCATION_GRAVE) or c:IsFaceup())
end
function c33981008.drcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and Duel.IsExistingMatchingCard(c33981008.cfilter,tp,LOCATION_MZONE+LOCATION_GRAVE,0,1,nil)
end
function c33981008.filter(c)
	return c:IsSetCard(0x106e) and c:GetCode()~=33981008 and c:IsType(TYPE_SPELL) and c:IsAbleToDeck()
end
function c33981008.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) and Duel.IsExistingMatchingCard(c33981008.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,1,tp,LOCATION_GRAVE)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c33981008.drop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectMatchingCard(tp,c33981008.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	if g:GetCount()>0 and Duel.SendtoDeck(g,nil,1,REASON_EFFECT)>0 then
		Duel.ConfirmCards(1-tp,g)
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
function c33981008.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsReason(REASON_DESTROY) and not e:GetHandler():IsReason(REASON_RULE) and rp==1-tp
end
function c33981008.ctfilter(c)
	return c:IsSetCard(0x106e) and c:IsType(TYPE_SPELL)
end
function c33981008.spfilter(c,e,tp,lv)
	return c:IsRace(RACE_SPELLCASTER) and c:IsLevelBelow(lv) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c33981008.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return false end
		local ct=Duel.GetMatchingGroupCount(c33981008.ctfilter,tp,LOCATION_GRAVE,0,nil)
		return Duel.IsExistingMatchingCard(c33981008.spfilter,tp,LOCATION_DECK+LOCATION_HAND,0,1,nil,e,tp,ct)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK+LOCATION_HAND)
end
function c33981008.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return false end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local ct=Duel.GetMatchingGroupCount(c33981008.ctfilter,tp,LOCATION_GRAVE,0,nil)
	local g=Duel.SelectMatchingCard(tp,c33981008.spfilter,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,e,tp,ct)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
