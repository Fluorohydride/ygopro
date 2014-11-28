--ジェムナイトレディ・ラピスラズリ
function c47611119.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCodeFun(c,99645428,aux.FilterBoolFunction(Card.IsSetCard,0x1047),1,false,false)
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c47611119.splimit)
	c:RegisterEffect(e1)
	--spsummon limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SPSUMMON_COST)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCost(c47611119.spcost)
	c:RegisterEffect(e2)
	--damage
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetTarget(c47611119.damtg)
	e3:SetOperation(c47611119.damop)
	c:RegisterEffect(e3)
	if not c47611119.global_check then
		c47611119.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON)
		ge1:SetOperation(aux.spchk1)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge2:SetOperation(aux.spchk1)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_CHAINING)
		ge3:SetOperation(aux.spchk2)
		Duel.RegisterEffect(ge3,0)
		local ge4=Effect.CreateEffect(c)
		ge4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge4:SetCode(EVENT_CHAIN_NEGATED)
		ge4:SetOperation(aux.spchk3)
		Duel.RegisterEffect(ge4,0)
	end
end
function c47611119.splimit(e,se,sp,st)
	return not e:GetHandler():IsLocation(LOCATION_EXTRA) or bit.band(st,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c47611119.spcost(e,c,tp)
	return Duel.GetFlagEffect(tp,47611119)==0
end
function c47611119.filter(c)
	return c:IsSetCard(0x1047) and c:IsType(TYPE_MONSTER) and c:IsAbleToGrave()
end
function c47611119.ctfilter(c)
	return bit.band(c:GetSummonType(),SUMMON_TYPE_SPECIAL)==SUMMON_TYPE_SPECIAL
end
function c47611119.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c47611119.filter,tp,LOCATION_DECK+LOCATION_EXTRA,0,1,nil)
		and Duel.IsExistingMatchingCard(c47611119.ctfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local ct=Duel.GetMatchingGroupCount(c47611119.ctfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,ct*500)
end
function c47611119.damop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c47611119.filter,tp,LOCATION_DECK+LOCATION_EXTRA,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
		local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
		local ct=Duel.GetMatchingGroupCount(c47611119.ctfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
		Duel.Damage(p,ct*500,REASON_EFFECT)
	end
end
