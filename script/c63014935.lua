--ヴォルカニック・クイーン
function c63014935.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
	e1:SetRange(LOCATION_HAND)
	e1:SetTargetRange(POS_FACEUP,1)
	e1:SetCondition(c63014935.spcon)
	e1:SetOperation(c63014935.spop)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(63014935,0))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c63014935.damcost)
	e2:SetTarget(c63014935.damtg)
	e2:SetOperation(c63014935.damop)
	c:RegisterEffect(e2)
	--release or damage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(63014935,1))
	e3:SetCategory(CATEGORY_RELEASE+CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c63014935.phcon)
	e3:SetOperation(c63014935.phop)
	c:RegisterEffect(e3)
	--spsummon cost
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_SPSUMMON_COST)
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e4:SetCost(c63014935.spcost)
	e4:SetOperation(c63014935.spcop)
	c:RegisterEffect(e4)
end
function c63014935.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(1-c:GetControler(),LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(Card.IsReleasable,c:GetControler(),0,LOCATION_MZONE,1,nil)
end
function c63014935.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local g=Duel.SelectMatchingCard(tp,Card.IsReleasable,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c63014935.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToGraveAsCost,tp,LOCATION_ONFIELD,0,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,Card.IsAbleToGraveAsCost,tp,LOCATION_ONFIELD,0,1,1,e:GetHandler())
	Duel.SendtoGrave(g,REASON_COST)
end
function c63014935.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,0,0,1-tp,1000)
end
function c63014935.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
function c63014935.phcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c63014935.phop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.CheckReleaseGroup(tp,aux.TRUE,1,e:GetHandler()) and Duel.SelectYesNo(tp,aux.Stringid(63014935,2)) then
		Duel.Release(Duel.SelectReleaseGroup(tp,aux.TRUE,1,1,e:GetHandler()),REASON_EFFECT)
	else Duel.Damage(tp,1000,REASON_EFFECT) end
end
function c63014935.spcost(e,c,tp)
	return Duel.GetActivityCount(tp,ACTIVITY_NORMALSUMMON)==0
end
function c63014935.spcop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetReset(RESET_PHASE+RESET_END)
	e1:SetTargetRange(1,0)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_MSET)
	Duel.RegisterEffect(e2,tp)
end
