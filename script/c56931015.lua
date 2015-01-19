--グラヴィティ・ベヒモス
function c56931015.initial_effect(c)
	--summon with no tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(56931015,0))
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c56931015.ntcon)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(56931015,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c56931015.descost)
	e2:SetTarget(c56931015.destg)
	e2:SetOperation(c56931015.desop)
	c:RegisterEffect(e2)
end
function c56931015.ntcon(e,c,minc)
	if c==nil then return Duel.GetTurnCount()~=1 end
	return minc==0 and c:GetLevel()>4 and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.GetFieldGroupCount(c:GetControler(),LOCATION_ONFIELD,LOCATION_ONFIELD)==0
end
function c56931015.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetAttackAnnouncedCount()==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_OATH)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e:GetHandler():RegisterEffect(e1)
end
function c56931015.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c1=Duel.GetFieldCard(0,LOCATION_SZONE,5)
	local c2=Duel.GetFieldCard(1,LOCATION_SZONE,5)
	if chk==0 then return (c1 and c1:IsDestructable()) or (c2 and c2:IsDestructable()) end
	local g=Group.CreateGroup()
	if c1 and c1:IsDestructable() then g:AddCard(c1) end
	if c2 and c2:IsDestructable() then g:AddCard(c2) end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c56931015.desop(e,tp,eg,ep,ev,re,r,rp)
	local c1=Duel.GetFieldCard(0,LOCATION_SZONE,5)
	local c2=Duel.GetFieldCard(1,LOCATION_SZONE,5)
	local g=Group.CreateGroup()
	if c1 and c1:IsDestructable() then g:AddCard(c1) end
	if c2 and c2:IsDestructable() then g:AddCard(c2) end
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
