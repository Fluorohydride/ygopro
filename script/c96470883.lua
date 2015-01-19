--凛天使クイーン・オブ・ローズ
function c96470883.initial_effect(c)
	--summon with 1 tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(96470883,0))
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c96470883.otcon)
	e1:SetOperation(c96470883.otop)
	e1:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(96470883,1))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c96470883.descon)
	e2:SetTarget(c96470883.destg)
	e2:SetOperation(c96470883.desop)
	c:RegisterEffect(e2)
end
function c96470883.otfilter(c,tp)
	return c:IsRace(RACE_PLANT) and (c:IsControler(tp) or c:IsFaceup())
end
function c96470883.otcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=Duel.GetMatchingGroup(c96470883.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	return c:GetLevel()>6 and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.GetTributeCount(c,mg)>0
end
function c96470883.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local mg=Duel.GetMatchingGroup(c96470883.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	local sg=Duel.SelectTribute(tp,c,1,1,mg)
	c:SetMaterial(sg)
	Duel.Release(sg,REASON_SUMMON+REASON_MATERIAL)
end
function c96470883.filter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c96470883.descon(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c96470883.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(c96470883.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		local tg=g:GetMinGroup(Card.GetAttack)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,tg,1,0,0)
	end
end
function c96470883.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c96470883.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		local tg=g:GetMinGroup(Card.GetAttack)
		if tg:GetCount()>1 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local sg=tg:Select(tp,1,1,nil)
			Duel.Destroy(sg,REASON_EFFECT)
		else Duel.Destroy(tg,REASON_EFFECT) end
	end
end
