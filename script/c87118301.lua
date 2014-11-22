--霊獣使い レラ
function c87118301.initial_effect(c)
	--spsummon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetCondition(c87118301.limcon)
	c:RegisterEffect(e1)
	--spsuccess
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(87118301,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetTarget(c87118301.sptg)
	e2:SetOperation(c87118301.spop)
	c:RegisterEffect(e2)
	if not c87118301.global_check then
		c87118301.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge1:SetOperation(c87118301.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c87118301.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	local p1=false
	local p2=false
	while tc do
		if tc:IsCode(87118301) then
			if tc:GetSummonPlayer()==0 then p1=true else p2=true end
		end
		tc=eg:GetNext()
	end
	if p1 then Duel.RegisterFlagEffect(0,87118301,RESET_PHASE+PHASE_END,0,1) end
	if p2 then Duel.RegisterFlagEffect(1,87118301,RESET_PHASE+PHASE_END,0,1) end
end
function c87118301.limcon(e)
	return Duel.GetFlagEffect(e:GetHandlerPlayer(),87118301)~=0
end
function c87118301.filter(c,e,tp)
	return c:IsSetCard(0xb5) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c87118301.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c87118301.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c87118301.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c87118301.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c87118301.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
