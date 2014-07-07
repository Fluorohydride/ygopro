--武神－アラスダ
function c23979249.initial_effect(c)
	c:SetUniqueOnField(1,0,23979249)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(23979249,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_REMOVE)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c23979249.spcon)
	e1:SetTarget(c23979249.sptg)
	e1:SetOperation(c23979249.spop)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_TO_HAND)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c23979249.regcon)
	e2:SetOperation(c23979249.regop)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(23979249,1))
	e3:SetCategory(CATEGORY_DRAW+CATEGORY_HANDES)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c23979249.drcon)
	e3:SetTarget(c23979249.drtg)
	e3:SetOperation(c23979249.drop)
	c:RegisterEffect(e3)
end
function c23979249.cfilter(c,tp)
	return c:IsSetCard(0x88) and c:GetPreviousControler()==tp
		and c:IsPreviousLocation(LOCATION_MZONE+LOCATION_GRAVE)
end
function c23979249.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c23979249.cfilter,1,nil,tp)
end
function c23979249.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c23979249.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
	end
end
function c23979249.regfilter(c,tp)
	return c:IsSetCard(0x88) and c:IsControler(tp) and c:GetPreviousControler()==tp
		and c:IsPreviousLocation(LOCATION_DECK) and not c:IsReason(REASON_DRAW)
end
function c23979249.regcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c23979249.regfilter,1,nil,tp)
end
function c23979249.regop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(23979249,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c23979249.drcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(23979249)>0
end
function c23979249.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,1)
end
function c23979249.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	if Duel.Draw(p,d,REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		Duel.DiscardHand(tp,nil,1,1,REASON_EFFECT+REASON_DISCARD)
	end
end
