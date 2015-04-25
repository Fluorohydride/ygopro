--ファーニマル・マウス
function c6142488.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_NO_TURN_RESET)
	e1:SetCountLimit(1)
	e1:SetCost(c6142488.spcost)
	e1:SetTarget(c6142488.sptg)
	e1:SetOperation(c6142488.spop)
	c:RegisterEffect(e1)
	Duel.AddCustomActivityCounter(6142488,ACTIVITY_SPSUMMON,c6142488.counterfilter)
end
function c6142488.counterfilter(c)
	return c:IsSetCard(0xad) or c:GetSummonLocation()~=LOCATION_EXTRA
end
function c6142488.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetCustomActivityCount(6142488,tp,ACTIVITY_SPSUMMON)==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c6142488.splimit)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
end
function c6142488.splimit(e,c,sump,sumtype,sumpos,targetp,se)
	return not c:IsSetCard(0xad) and c:IsLocation(LOCATION_EXTRA)
end
function c6142488.filter(c,e,tp)
	return c:IsCode(6142488) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c6142488.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c6142488.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c6142488.spop(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ct==0 then return end
	if ct>2 then ct=2 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c6142488.filter,tp,LOCATION_DECK,0,1,ct,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
