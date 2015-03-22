--エクストラ・ヴェーラー
function c32391566.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(32391566,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c32391566.spcon)
	e1:SetTarget(c32391566.sptg)
	e1:SetOperation(c32391566.spop)
	c:RegisterEffect(e1)
end
function c32391566.cfilter(c,tp)
	return c:GetSummonPlayer()==tp
end
function c32391566.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c32391566.cfilter,1,nil,1-tp)
end
function c32391566.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c32391566.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetCode(EFFECT_REFLECT_DAMAGE)
		e1:SetTargetRange(1,0)
		e1:SetValue(c32391566.val)
		e1:SetReset(RESET_PHASE+PHASE_END)
		Duel.RegisterEffect(e1,tp)
	end
end
function c32391566.val(e,re,ev,r,rp,rc)
	return bit.band(r,REASON_EFFECT)~=0
end
