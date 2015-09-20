--ナイトメア・デーモンズ
function c42956963.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetCost(c42956963.cost)
	e1:SetTarget(c42956963.target)
	e1:SetOperation(c42956963.activate)
	c:RegisterEffect(e1)
end
function c42956963.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,nil,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,nil,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c42956963.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(1-tp,LOCATION_MZONE)>2
		and Duel.IsPlayerCanSpecialSummonMonster(tp,42956964,0x45,0x4011,2000,2000,6,RACE_FIEND,ATTRIBUTE_DARK,POS_FACEUP_ATTACK,1-tp) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,3,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,3,0,0)
end
function c42956963.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE)<3 then return end
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,42956964,0x45,0x4011,2000,2000,6,RACE_FIEND,ATTRIBUTE_DARK,POS_FACEUP_ATTACK,1-tp) then return end
	local g=Group.CreateGroup()
	for i=1,3 do
		local token=Duel.CreateToken(tp,42956964)
		if Duel.SpecialSummonStep(token,0,tp,1-tp,false,false,POS_FACEUP_ATTACK) then
			g:AddCard(token)
		end
	end
	g:KeepAlive()
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetLabelObject(g)
	e2:SetCondition(c42956963.damcon)
	e2:SetOperation(c42956963.damop)
	Duel.RegisterEffect(e2,tp)
	Duel.SpecialSummonComplete()
end
function c42956963.dfilter(c,g)
	return g:IsContains(c)
end
function c42956963.damcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if eg:IsExists(c42956963.dfilter,1,nil,g) then
		return true
	else
		if not g:IsExists(Card.IsLocation,1,nil,LOCATION_MZONE) then
			g:DeleteGroup()
			e:Reset()
		end
		return false
	end
end
function c42956963.damop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tc=eg:GetFirst()
	while tc do
		if g:IsContains(tc) then
			Duel.Damage(tc:GetPreviousControler(),800,REASON_EFFECT)
			g:RemoveCard(tc)
		end
		tc=eg:GetNext()
	end
end
